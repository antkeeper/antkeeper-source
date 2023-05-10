/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <engine/render/passes/material-pass.hpp>
#include <engine/config.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-attribute.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/gl/shader-variable-type.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/render/model.hpp>
#include <engine/render/context.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/ambient-light.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/scene/spot-light.hpp>
#include <engine/scene/sphere-light.hpp>
#include <engine/config.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/projection.hpp>
#include <engine/utility/hash/combine.hpp>
#include <cmath>
#include <glad/glad.h>
#include <execution>

namespace render {

namespace {

/**
 * Sorts render operations for the material pass.
 */
bool operation_compare(const render::operation* a, const render::operation* b)
{
	// Render operations with materials first
	if (!a->material)
	{
		return false;
	}
	else if (!b->material)
	{
		return true;
	}
	
	const bool translucent_a = a->material->get_blend_mode() == material_blend_mode::translucent;
	const bool translucent_b = b->material->get_blend_mode() == material_blend_mode::translucent;
	
	if (translucent_a)
	{
		if (translucent_b)
		{
			// A and B are both translucent, render back to front
			return (a->depth < b->depth);
		}
		else
		{
			// A is translucent, B is opaque. Render B first
			return false;
		}
	}
	else
	{
		if (translucent_b)
		{
			// A is opaque, B is translucent. Render A first
			return true;
		}
		else
		{
			// A and B are both opaque
			
			const std::size_t hash_a = a->material->hash();
			const std::size_t hash_b = b->material->hash();
			
			if (hash_a == hash_b)
			{
				// A and B have same material hash, sort by VAO
				return (a->vertex_array < b->vertex_array);
			}
			else
			{
				// A and B have different material hashes, sort by hash
				return (hash_a < hash_b);
			}
		}
	}
}

} // namespace

material_pass::material_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer)
{}

void material_pass::render(render::context& ctx)
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_GREATER);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_STENCIL_TEST);
	
	// For half-z buffer
	glDepthRange(-1.0f, 1.0f);
	
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	//const gl::shader_program* active_shader_program = nullptr;
	const render::material* active_material = nullptr;
	std::size_t active_material_hash = 0;
	bool active_two_sided = false;
	material_blend_mode active_blend_mode = material_blend_mode::opaque;
	std::size_t active_cache_key = 0;
	shader_cache_entry* active_cache_entry = nullptr;
	
	// Gather information
	evaluate_camera(ctx);
	evaluate_lighting(ctx);
	evaluate_misc(ctx);
	
	// Sort render operations
	std::sort(std::execution::par_unseq, ctx.operations.begin(), ctx.operations.end(), operation_compare);
	
	for (const render::operation* operation: ctx.operations)
	{
		// Get operation material
		const render::material* material = operation->material.get();
		if (!material)
		{
			if (!fallback_material)
			{
				// No material specified and no fallback material, skip operation
				continue;
			}
			
			// Use fallback material
			material = fallback_material.get();
		}
		
		// Switch materials if necessary
		if (active_material != material)
		{
			if (!material->get_shader_template())
			{
				continue;
			}
			
			if (active_material_hash != material->hash())
			{
				// Set culling mode
				if (active_two_sided != material->is_two_sided())
				{
					if (material->is_two_sided())
					{
						glDisable(GL_CULL_FACE);
					}
					else
					{
						glEnable(GL_CULL_FACE);
					}
					
					active_two_sided = material->is_two_sided();
				}
				
				// Set blend mode
				if (active_blend_mode != material->get_blend_mode())
				{
					if (material->get_blend_mode() == material_blend_mode::translucent)
					{
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
					else
					{
						glDisable(GL_BLEND);
					}
					
					active_blend_mode = material->get_blend_mode();
				}
				
				active_material_hash = material->hash();
			}
			
			// Calculate shader cache key
			std::size_t cache_key = hash::combine(lighting_state_hash, material->get_shader_template()->hash());
			if (active_cache_key != cache_key)
			{
				// Lookup shader cache entry
				if (auto i = shader_cache.find(cache_key); i != shader_cache.end())
				{
					active_cache_entry = &i->second;
				}
				else
				{
					// Construct cache entry
					active_cache_entry = &shader_cache[cache_key];
					active_cache_entry->shader_program = generate_shader_program(*material->get_shader_template());
					build_shader_command_buffer(active_cache_entry->shader_command_buffer, *active_cache_entry->shader_program);
					build_geometry_command_buffer(active_cache_entry->geometry_command_buffer, *active_cache_entry->shader_program);
					
					debug::log::trace("Generated material cache entry {:x}", cache_key);
				}
				
				// Bind shader and update shader-specific variables
				for (const auto& command: active_cache_entry->shader_command_buffer)
				{
					command();
				}
				
				active_cache_key = cache_key;
			}
			
			// Find or build material command buffer
			std::vector<std::function<void()>>* material_command_buffer;
			if (auto i = active_cache_entry->material_command_buffers.find(material); i != active_cache_entry->material_command_buffers.end())
			{
				material_command_buffer = &i->second;
			}
			else
			{
				material_command_buffer = &active_cache_entry->material_command_buffers[material];
				build_material_command_buffer(*material_command_buffer, *active_cache_entry->shader_program, *material);
				
				debug::log::trace("Generated material command buffer");
			}
			
			// Update material-dependent shader variables
			for (const auto& command: *material_command_buffer)
			{
				command();
			}
			
			active_material = material;
		}
		
		// Update geometry-dependent shader variables
		model = &operation->transform;
		matrix_palette = operation->matrix_palette;
		for (const auto& command: active_cache_entry->geometry_command_buffer)
		{
			command();
		}
		
		// Draw geometry
		if (operation->instance_count)
		{
			rasterizer->draw_arrays_instanced(*operation->vertex_array, operation->drawing_mode, operation->start_index, operation->index_count, operation->instance_count);
		}
		else
		{
			rasterizer->draw_arrays(*operation->vertex_array, operation->drawing_mode, operation->start_index, operation->index_count);
		}
	}
	
	++frame;
}

void material_pass::set_fallback_material(std::shared_ptr<render::material> fallback)
{
	this->fallback_material = fallback;
}

void material_pass::evaluate_camera(const render::context& ctx)
{
	view = &ctx.camera->get_view();
	projection = &ctx.camera->get_projection();
	view_projection = &ctx.camera->get_view_projection();
	camera_position = &ctx.camera->get_translation();
	camera_exposure = ctx.camera->get_exposure_normalization();
	clip_depth =
	{
		ctx.camera->get_clip_near(),
		ctx.camera->get_clip_far()
	};
	log_depth_coef = 2.0f / std::log2(clip_depth[1] + 1.0f);
}

void material_pass::evaluate_lighting(const render::context& ctx)
{
	// Reset light and shadow counts
	ambient_light_count = 0;
	point_light_count = 0;
	directional_light_count = 0;
	directional_shadow_count = 0;
	spot_light_count = 0;
	sphere_light_count = 0;
	
	const auto& lights = ctx.collection->get_objects(scene::light::object_type_id);
	for (const scene::object_base* object: lights)
	{
		const scene::light& light = static_cast<const scene::light&>(*object);
		
		switch (light.get_light_type())
		{
			// Add ambient light
			case scene::light_type::ambient:
			{
				const std::size_t index = ambient_light_count;
				
				++ambient_light_count;
				if (ambient_light_count > ambient_light_colors.size())
				{
					ambient_light_colors.resize(ambient_light_count);
				}
				
				
				
				ambient_light_colors[index] = static_cast<const scene::ambient_light&>(light).get_illuminance() * ctx.camera->get_exposure_normalization();
				break;
			}
			
			// Add directional light
			case scene::light_type::directional:
			{
				const scene::directional_light& directional_light = static_cast<const scene::directional_light&>(light);
				
				const std::size_t index = directional_light_count;
				
				++directional_light_count;
				if (directional_light_count > directional_light_colors.size())
				{
					directional_light_colors.resize(directional_light_count);
					directional_light_directions.resize(directional_light_count);
				}
				
				directional_light_colors[index] = directional_light.get_illuminance() * ctx.camera->get_exposure_normalization();
				directional_light_directions[index] = directional_light.get_direction();
				
				// Add directional shadow
				if (directional_light.is_shadow_caster() && directional_light.get_shadow_framebuffer())
				{
					const std::size_t index = directional_shadow_count;
					
					++directional_shadow_count;
					if (directional_shadow_count > directional_shadow_maps.size())
					{
						directional_shadow_maps.resize(directional_shadow_count);
						directional_shadow_biases.resize(directional_shadow_count);
						directional_shadow_splits.resize(directional_shadow_count);
						directional_shadow_matrices.resize(directional_shadow_count);
					}
					
					directional_shadow_maps[index] = directional_light.get_shadow_framebuffer()->get_depth_attachment();
					directional_shadow_biases[index] = directional_light.get_shadow_bias();
					directional_shadow_splits[index] = &directional_light.get_shadow_cascade_distances();
					directional_shadow_matrices[index] = &directional_light.get_shadow_cascade_matrices();
				}
				break;
			}
			
			// Add spot_light
			case scene::light_type::spot:
			{
				const scene::spot_light& spot_light = static_cast<const scene::spot_light&>(light);
				
				const std::size_t index = spot_light_count;
				
				++spot_light_count;
				if (spot_light_count > spot_light_colors.size())
				{
					spot_light_colors.resize(spot_light_count);
					spot_light_positions.resize(spot_light_count);
					spot_light_directions.resize(spot_light_count);
					spot_light_cutoffs.resize(spot_light_count);
				}
				
				spot_light_colors[index] = spot_light.get_luminous_flux() * ctx.camera->get_exposure_normalization();
				spot_light_positions[index] = spot_light.get_translation();
				spot_light_directions[index] = spot_light.get_direction();
				spot_light_cutoffs[index] = spot_light.get_cosine_cutoff();
				break;
			}
			
			// Add sphere light
			case scene::light_type::sphere:
			{
				const scene::sphere_light& sphere_light = static_cast<const scene::sphere_light&>(light);
				
				if (sphere_light.get_radius() == 0.0f)
				{
					const std::size_t index = point_light_count;
					
					++point_light_count;
					if (point_light_count > point_light_colors.size())
					{
						point_light_colors.resize(point_light_count);
						point_light_positions.resize(point_light_count);
					}
					
					point_light_colors[index] = sphere_light.get_luminous_flux() * ctx.camera->get_exposure_normalization();
					point_light_positions[index] = sphere_light.get_translation();
				}
				else
				{
					const std::size_t index = sphere_light_count;
					
					++sphere_light_count;
					if (sphere_light_count > sphere_light_colors.size())
					{
						sphere_light_colors.resize(sphere_light_count);
						sphere_light_positions_radii.resize(sphere_light_count);
					}
					
					sphere_light_colors[index] = sphere_light.get_luminous_flux() * ctx.camera->get_exposure_normalization();
					
					const auto& position = sphere_light.get_translation();
					auto& position_radius = sphere_light_positions_radii[index];
					position_radius[0] = position.x();
					position_radius[1] = position.y();
					position_radius[2] = position.z();
					position_radius[3] = sphere_light.get_radius();
				}
				
				break;
			}
			
			default:
				break;
		}
	}
	
	// Generate lighting state hash
	lighting_state_hash = std::hash<std::size_t>{}(ambient_light_count);
	lighting_state_hash = hash::combine(lighting_state_hash, std::hash<std::size_t>{}(directional_light_count));
	lighting_state_hash = hash::combine(lighting_state_hash, std::hash<std::size_t>{}(directional_shadow_count));
	lighting_state_hash = hash::combine(lighting_state_hash, std::hash<std::size_t>{}(point_light_count));
	lighting_state_hash = hash::combine(lighting_state_hash, std::hash<std::size_t>{}(spot_light_count));
	lighting_state_hash = hash::combine(lighting_state_hash, std::hash<std::size_t>{}(sphere_light_count));
}

void material_pass::evaluate_misc(const render::context& ctx)
{
	time = ctx.t;
	timestep = ctx.dt;
	subframe = ctx.alpha;
	
	const auto viewport_size = framebuffer->get_dimensions();
	resolution = 
	{
		static_cast<float>(std::get<0>(viewport_size)),
		static_cast<float>(std::get<1>(viewport_size))
	};
	
	///mouse_position = ...
}

std::unique_ptr<gl::shader_program> material_pass::generate_shader_program(const gl::shader_template& shader_template) const
{
	std::unordered_map<std::string, std::string> definitions;
	
	definitions["VERTEX_POSITION"]    = std::to_string(vertex_attribute::position);
	definitions["VERTEX_UV"]          = std::to_string(vertex_attribute::uv);
	definitions["VERTEX_NORMAL"]      = std::to_string(vertex_attribute::normal);
	definitions["VERTEX_TANGENT"]     = std::to_string(vertex_attribute::tangent);
	definitions["VERTEX_COLOR"]       = std::to_string(vertex_attribute::color);
	definitions["VERTEX_BONE_INDEX"]  = std::to_string(vertex_attribute::bone_index);
	definitions["VERTEX_BONE_WEIGHT"] = std::to_string(vertex_attribute::bone_weight);
	definitions["VERTEX_BARYCENTRIC"] = std::to_string(vertex_attribute::barycentric);
	definitions["VERTEX_TARGET"]      = std::to_string(vertex_attribute::target);
	
	definitions["FRAGMENT_OUTPUT_COLOR"] = std::to_string(0);
	
	definitions["AMBIENT_LIGHT_COUNT"] = std::to_string(ambient_light_count);
	definitions["DIRECTIONAL_LIGHT_COUNT"] = std::to_string(directional_light_count);
	definitions["DIRECTIONAL_SHADOW_COUNT"] = std::to_string(directional_shadow_count);
	definitions["POINT_LIGHT_COUNT"] = std::to_string(point_light_count);
	definitions["SPOT_LIGHT_COUNT"] = std::to_string(spot_light_count);
	definitions["SPHERE_LIGHT_COUNT"] = std::to_string(sphere_light_count);
	
	auto shader_program = shader_template.build(definitions);
	
	if (!shader_program->linked())
	{
		debug::log::error("Failed to link material shader program: {}", shader_program->info());
		debug::log::warning("{}", shader_template.configure(gl::shader_stage::fragment, definitions));
	}
	
	return shader_program;
}

void material_pass::build_shader_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const
{
	// Bind shader program
	command_buffer.emplace_back([&](){rasterizer->use_program(shader_program);});
	
	// Update camera variables
	if (auto view_var = shader_program.variable("view"))
	{
		command_buffer.emplace_back([&, view_var](){view_var->update(*view);});
	}
	if (auto projection_var = shader_program.variable("projection"))
	{
		command_buffer.emplace_back([&, projection_var](){projection_var->update(*projection);});
	}
	if (auto view_projection_var = shader_program.variable("view_projection"))
	{
		command_buffer.emplace_back([&, view_projection_var](){view_projection_var->update(*view_projection);});
	}
	if (auto camera_position_var = shader_program.variable("camera_position"))
	{
		command_buffer.emplace_back([&, camera_position_var](){camera_position_var->update(*camera_position);});
	}
	if (auto camera_exposure_var = shader_program.variable("camera_exposure"))
	{
		command_buffer.emplace_back([&, camera_exposure_var](){camera_exposure_var->update(camera_exposure);});
	}
	if (auto clip_depth_var = shader_program.variable("clip_depth"))
	{
		command_buffer.emplace_back([&, clip_depth_var](){clip_depth_var->update(clip_depth);});
	}
	
	// Update ambient light variables
	if (ambient_light_count)
	{
		if (auto ambient_light_colors_var = shader_program.variable("ambient_light_colors"))
		{
			command_buffer.emplace_back([&, ambient_light_colors_var](){ambient_light_colors_var->update(std::span<const float3>{ambient_light_colors.data(), ambient_light_count});});
		}
	}
	
	// Update directional light variables
	if (directional_light_count)
	{
		if (auto directional_light_colors_var = shader_program.variable("directional_light_colors"))
		{
			if (auto directional_light_directions_var = shader_program.variable("directional_light_directions"))
			{
				command_buffer.emplace_back
				(
					[&, directional_light_colors_var, directional_light_directions_var]()
					{
						directional_light_colors_var->update(std::span<const float3>{directional_light_colors.data(), directional_light_count});
						directional_light_directions_var->update(std::span<const float3>{directional_light_directions.data(), directional_light_count});
					}
				);
			}
		}
	}
	
	// Update directional shadow variables
	if (directional_shadow_count)
	{
		if (auto directional_shadow_maps_var = shader_program.variable("directional_shadow_maps"))
		{
			auto directional_shadow_biases_var = shader_program.variable("directional_shadow_biases");
			auto directional_shadow_splits_var = shader_program.variable("directional_shadow_splits");
			auto directional_shadow_matrices_var = shader_program.variable("directional_shadow_matrices");
			
			if (directional_shadow_maps_var && directional_shadow_biases_var && directional_shadow_splits_var && directional_shadow_matrices_var)
			{
				command_buffer.emplace_back
				(
					[&, directional_shadow_maps_var, directional_shadow_biases_var, directional_shadow_splits_var, directional_shadow_matrices_var]()
					{
						directional_shadow_maps_var->update(std::span<const gl::texture_2d* const>{directional_shadow_maps.data(), directional_shadow_count});
						directional_shadow_biases_var->update(std::span<const float>{directional_shadow_biases.data(), directional_shadow_count});
						
						std::size_t offset = 0;
						for (std::size_t i = 0; i < directional_shadow_count; ++i)
						{
							directional_shadow_splits_var->update(*directional_shadow_splits[i], offset * 4);
							directional_shadow_matrices_var->update(*directional_shadow_matrices[i], offset);
							offset += directional_shadow_splits[i]->size();
						}
					}
				);
			}
		}
	}
	
	// Update point light variables
	if (point_light_count)
	{
		if (auto point_light_colors_var = shader_program.variable("point_light_colors"))
		{
			auto point_light_positions_var = shader_program.variable("point_light_positions");
			
			if (point_light_positions_var)
			{
				command_buffer.emplace_back
				(
					[&, point_light_colors_var, point_light_positions_var]()
					{
						point_light_colors_var->update(std::span<const float3>{point_light_colors.data(), point_light_count});
						point_light_positions_var->update(std::span<const float3>{point_light_positions.data(), point_light_count});
					}
				);
			}
		}
	}
	
	// Update spot light variables
	if (spot_light_count)
	{
		if (auto spot_light_colors_var = shader_program.variable("spot_light_colors"))
		{
			auto spot_light_positions_var = shader_program.variable("spot_light_positions");
			auto spot_light_directions_var = shader_program.variable("spot_light_directions");
			auto spot_light_cutoffs_var = shader_program.variable("spot_light_cutoffs");
			
			if (spot_light_positions_var && spot_light_directions_var && spot_light_cutoffs_var)
			{
				command_buffer.emplace_back
				(
					[&, spot_light_colors_var, spot_light_positions_var, spot_light_directions_var, spot_light_cutoffs_var]()
					{
						spot_light_colors_var->update(std::span<const float3>{spot_light_colors.data(), spot_light_count});
						spot_light_positions_var->update(std::span<const float3>{spot_light_positions.data(), spot_light_count});
						spot_light_directions_var->update(std::span<const float3>{spot_light_directions.data(), spot_light_count});
						spot_light_cutoffs_var->update(std::span<const float2>{spot_light_cutoffs.data(), spot_light_count});
					}
				);
			}
		}
	}
	
	// Update sphere light variables
	if (sphere_light_count)
	{
		if (auto sphere_light_colors_var = shader_program.variable("sphere_light_colors"))
		{
			auto sphere_light_positions_radii_var = shader_program.variable("sphere_light_positions_radii");
			
			if (sphere_light_positions_radii_var)
			{
				command_buffer.emplace_back
				(
					[&, sphere_light_colors_var, sphere_light_positions_radii_var]()
					{
						sphere_light_colors_var->update(std::span<const float3>{sphere_light_colors.data(), sphere_light_count});
						sphere_light_positions_radii_var->update(std::span<const float4>{sphere_light_positions_radii.data(), sphere_light_count});
					}
				);
			}
		}
	}
	
	// Update time variable
	if (auto time_var = shader_program.variable("time"))
	{
		command_buffer.emplace_back([&, time_var](){time_var->update(time);});
	}
	
	// Update timestep variable
	if (auto timestep_var = shader_program.variable("timestep"))
	{
		command_buffer.emplace_back([&, timestep_var](){timestep_var->update(timestep);});
	}
	
	// Update frame variable
	if (auto frame_var = shader_program.variable("frame"))
	{
		command_buffer.emplace_back([&, frame_var](){frame_var->update(frame);});
	}
	
	// Update subframe variable
	if (auto subframe_var = shader_program.variable("subframe"))
	{
		command_buffer.emplace_back([&, subframe_var](){subframe_var->update(subframe);});
	}
	
	// Update resolution variable
	if (auto resolution_var = shader_program.variable("resolution"))
	{
		command_buffer.emplace_back([&, resolution_var](){resolution_var->update(resolution);});
	}
	
	// Update mouse position variable
	if (auto mouse_position_var = shader_program.variable("mouse_position"))
	{
		command_buffer.emplace_back([&, mouse_position_var](){mouse_position_var->update(mouse_position);});
	}
}

void material_pass::build_geometry_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const
{
	// Update model matrix variable
	if (auto model_var = shader_program.variable("model"))
	{
		command_buffer.emplace_back([&, model_var](){model_var->update(*model);});
	}
	
	// Update normal-model matrix variable
	if (auto normal_model_var = shader_program.variable("normal_model"))
	{
		command_buffer.emplace_back
		(
			[&, normal_model_var]()
			{
				normal_model_var->update(math::transpose(math::inverse(math::matrix<float, 3, 3>(*model))));
			}
		);
	}
	
	// Update model-view matrix and normal-model-view matrix variables
	auto model_view_var = shader_program.variable("model_view");
	auto normal_model_view_var = shader_program.variable("normal_model_view");
	if (model_view_var && normal_model_view_var)
	{
		command_buffer.emplace_back
		(
			[&, model_view_var, normal_model_view_var]()
			{
				const auto model_view = (*view) * (*model);
				model_view_var->update(model_view);
				normal_model_view_var->update(math::transpose(math::inverse(math::matrix<float, 3, 3>(model_view))));
			}
		);
	}
	else
	{
		if (model_view_var)
		{
			command_buffer.emplace_back([&, model_view_var](){model_view_var->update((*view) * (*model));});
		}
		else if (normal_model_view_var)
		{
			command_buffer.emplace_back
			(
				[&, normal_model_view_var]()
				{
					const auto model_view = (*view) * (*model);
					normal_model_view_var->update(math::transpose(math::inverse(math::matrix<float, 3, 3>(model_view))));
				}
			);
		}
	}
	
	// Update model-view-projection matrix variable
	if (auto model_view_projection_var = shader_program.variable("model_view_projection"))
	{
		command_buffer.emplace_back([&, model_view_projection_var](){model_view_projection_var->update((*view_projection) * (*model));});
	}
	
	// Update matrix palette variable
	if (auto matrix_palette_var = shader_program.variable("matrix_palette"))
	{
		command_buffer.emplace_back([&, matrix_palette_var](){matrix_palette_var->update(matrix_palette);});
	}
}

void material_pass::build_material_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program, const material& material) const
{
	for (const auto& [key, material_var]: material.get_variables())
	{
		if (!material_var)
		{
			continue;
		}
		
		const auto shader_var = shader_program.variable(key);
		if (!shader_var)
		{
			continue;
		}
		
		const std::size_t size = std::min<std::size_t>(material_var->size(), shader_var->size());
		
		switch (shader_var->type())
		{
			/// @TODO render::material_bool is broken due to the std::vector<bool> specialization.
			// case gl::shader_variable_type::bool1:
				// if (material_var->type() == material_variable_type::bool1)
				// {
					// command_buffer.emplace_back
					// (
						// [size, shader_var, material_var = std::static_pointer_cast<material_bool>(material_var)]()
						// {
							// shader_var->update(std::span<const bool>{material_var->data(), size});
						// }
					// );
				// }
				// break;
			case gl::shader_variable_type::bool2:
				if (material_var->type() == material_variable_type::bool2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_bool2>(material_var)]()
						{
							shader_var->update(std::span<const bool2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::bool3:
				if (material_var->type() == material_variable_type::bool3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_bool3>(material_var)]()
						{
							shader_var->update(std::span<const bool3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::bool4:
				if (material_var->type() == material_variable_type::bool4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_bool4>(material_var)]()
						{
							shader_var->update(std::span<const bool4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::int1:
				if (material_var->type() == material_variable_type::int1)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_int>(material_var)]()
						{
							shader_var->update(std::span<const int>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::int2:
				if (material_var->type() == material_variable_type::int2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_int2>(material_var)]()
						{
							shader_var->update(std::span<const int2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::int3:
				if (material_var->type() == material_variable_type::int3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_int3>(material_var)]()
						{
							shader_var->update(std::span<const int3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::int4:
				if (material_var->type() == material_variable_type::int4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_int4>(material_var)]()
						{
							shader_var->update(std::span<const int4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uint1:
				if (material_var->type() == material_variable_type::uint1)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_uint>(material_var)]()
						{
							shader_var->update(std::span<const unsigned int>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uint2:
				if (material_var->type() == material_variable_type::uint2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_uint2>(material_var)]()
						{
							shader_var->update(std::span<const uint2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uint3:
				if (material_var->type() == material_variable_type::uint3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_uint3>(material_var)]()
						{
							shader_var->update(std::span<const uint3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uint4:
				if (material_var->type() == material_variable_type::uint4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_uint4>(material_var)]()
						{
							shader_var->update(std::span<const uint4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float1:
				if (material_var->type() == material_variable_type::float1)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float>(material_var)]()
						{
							shader_var->update(std::span<const float>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float2:
				if (material_var->type() == material_variable_type::float2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float2>(material_var)]()
						{
							shader_var->update(std::span<const float2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float3:
				if (material_var->type() == material_variable_type::float3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float3>(material_var)]()
						{
							shader_var->update(std::span<const float3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float4:
				if (material_var->type() == material_variable_type::float4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float4>(material_var)]()
						{
							shader_var->update(std::span<const float4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float2x2:
				if (material_var->type() == material_variable_type::float2x2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float2x2>(material_var)]()
						{
							shader_var->update(std::span<const float2x2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float3x3:
				if (material_var->type() == material_variable_type::float3x3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float3x3>(material_var)]()
						{
							shader_var->update(std::span<const float3x3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::float4x4:
				if (material_var->type() == material_variable_type::float4x4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_float4x4>(material_var)]()
						{
							shader_var->update(std::span<const float4x4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::texture_1d:
				if (material_var->type() == material_variable_type::texture_1d)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_texture_1d>(material_var)]()
						{
							shader_var->update(std::span<const std::shared_ptr<gl::texture_1d>>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::texture_2d:
				if (material_var->type() == material_variable_type::texture_2d)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_texture_2d>(material_var)]()
						{
							shader_var->update(std::span<const std::shared_ptr<gl::texture_2d>>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::texture_3d:
				if (material_var->type() == material_variable_type::texture_3d)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_texture_3d>(material_var)]()
						{
							shader_var->update(std::span<const std::shared_ptr<gl::texture_3d>>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::texture_cube:
				if (material_var->type() == material_variable_type::texture_cube)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<material_texture_cube>(material_var)]()
						{
							shader_var->update(std::span<const std::shared_ptr<gl::texture_cube>>{material_var->data(), size});
						}
					);
				}
				break;
			default:
				break;
		}
	}
}

} // namespace render

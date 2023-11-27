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
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture.hpp>
#include <engine/gl/shader-variable-type.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/render/model.hpp>
#include <engine/render/context.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/scene/spot-light.hpp>
#include <engine/scene/point-light.hpp>
#include <engine/scene/rectangle-light.hpp>
#include <engine/scene/light-probe.hpp>
#include <engine/config.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/projection.hpp>
#include <engine/utility/hash/hash-combine.hpp>
#include <cmath>
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
			// A and B are both opaque, sort by material hash
			const std::size_t hash_a = a->material->hash();
			const std::size_t hash_b = b->material->hash();
			
			if (hash_a != hash_b)
			{
				// A and B have different material hashes, sort by hash
				return (hash_a < hash_b);
			}
			else
			{
				// A and B have the same material hash, sort by layer mask
				if (a->layer_mask != b->layer_mask)
				{
					return (a->layer_mask < b->layer_mask);
				}
				else
				{
					// A and B have the same layer mask, sort by VAO
					return (a->vertex_array < b->vertex_array);
				}
			}
		}
	}
}

} // namespace

material_pass::material_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(pipeline, framebuffer)
{
	// Load LTC LUT textures
	ltc_lut_1 = resource_manager->load<gl::texture_2d>("ltc-lut-1.tex");
	ltc_lut_2 = resource_manager->load<gl::texture_2d>("ltc-lut-2.tex");
	
	// Load IBL BRDF LUT texture
	brdf_lut = resource_manager->load<gl::texture_2d>("brdf-lut.tex");
}

void material_pass::render(render::context& ctx)
{
	m_pipeline->bind_framebuffer(m_framebuffer);
	
	m_pipeline->set_color_blend_enabled(false);
	m_pipeline->set_depth_test_enabled(true);
	m_pipeline->set_depth_compare_op(gl::compare_op::greater_or_equal);
	m_pipeline->set_cull_mode(gl::cull_mode::back);
	m_pipeline->set_stencil_test_enabled(false);
	
	const auto& viewport_dimensions = (m_framebuffer) ? m_framebuffer->dimensions() : m_pipeline->get_default_framebuffer_dimensions();
	const gl::viewport viewport[1] =
	{{
		0,
		0,
		static_cast<float>(viewport_dimensions[0]),
		static_cast<float>(viewport_dimensions[1])
	}};
	m_pipeline->set_viewport(0, viewport);
	
	//const gl::shader_program* active_shader_program = nullptr;
	const render::material* active_material = nullptr;
	std::size_t active_material_hash = 0;
	bool active_two_sided = false;
	material_blend_mode active_blend_mode = material_blend_mode::opaque;
	std::size_t active_cache_key = 0;
	shader_cache_entry* active_cache_entry = nullptr;
	std::uint32_t active_layer_mask = 0;
	std::size_t active_lighting_state_hash = 0;
	
	// Gather information
	evaluate_camera(ctx);
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
		
		// Evaluate visible lights
		if (active_layer_mask != operation->layer_mask)
		{
			evaluate_lighting(ctx, operation->layer_mask & ctx.camera->get_layer_mask());
			active_layer_mask = operation->layer_mask;
		}
		
		// Switch materials if necessary
		if (active_material != material || active_lighting_state_hash != lighting_state_hash)
		{
			// if (!material->get_shader_template())
			// {
				// continue;
			// }
			
			if (active_material_hash != material->hash())
			{
				// Set culling mode
				if (active_two_sided != material->is_two_sided())
				{
					if (material->is_two_sided())
					{
						m_pipeline->set_cull_mode(gl::cull_mode::none);
					}
					else
					{
						m_pipeline->set_cull_mode(gl::cull_mode::back);
					}
					
					active_two_sided = material->is_two_sided();
				}
				
				// Set blend mode
				if (active_blend_mode != material->get_blend_mode())
				{
					if (material->get_blend_mode() == material_blend_mode::translucent)
					{
						m_pipeline->set_color_blend_enabled(true);
						m_pipeline->set_color_blend_equation
						({
							gl::blend_factor::src_alpha,
							gl::blend_factor::one_minus_src_alpha,
							gl::blend_op::add,
							gl::blend_factor::src_alpha,
							gl::blend_factor::one_minus_src_alpha,
							gl::blend_op::add
						});
					}
					else
					{
						m_pipeline->set_color_blend_enabled(false);
					}
					
					active_blend_mode = material->get_blend_mode();
				}
				
				active_material_hash = material->hash();
			}
			
			// Calculate shader cache key
			std::size_t cache_key = hash_combine(lighting_state_hash, material->get_shader_template()->hash());
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
					active_cache_entry->shader_program = generate_shader_program(*material->get_shader_template(), material->get_blend_mode());
					build_shader_command_buffer(active_cache_entry->shader_command_buffer, *active_cache_entry->shader_program);
					build_geometry_command_buffer(active_cache_entry->geometry_command_buffer, *active_cache_entry->shader_program);
					
					debug::log_trace("Generated material cache entry {:x}", cache_key);
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
				
				debug::log_trace("Generated material command buffer");
			}
			
			// Update material-dependent shader variables
			for (const auto& command: *material_command_buffer)
			{
				command();
			}
			
			active_material = material;
			active_lighting_state_hash = lighting_state_hash;
		}
		
		
		model = &operation->transform;
		
		// @see Persson, E., & Studios, A. (2012). Creating vast game worlds: Experiences from avalanche studios. In ACM SIGGRAPH 2012 Talks (pp. 1-1).
		model_view = *model;
		model_view[3] -= view_translation;
		model_view = view_rotation * model_view;
		// model_view = (*view) * (*model);
		
		matrix_palette = operation->matrix_palette;
		
		// Update geometry-dependent shader variables
		for (const auto& command: active_cache_entry->geometry_command_buffer)
		{
			command();
		}
		
		m_pipeline->set_primitive_topology(operation->primitive_topology);
		m_pipeline->bind_vertex_array(operation->vertex_array);
		m_pipeline->bind_vertex_buffers(0, {&operation->vertex_buffer, 1}, {&operation->vertex_offset, 1}, {&operation->vertex_stride, 1});
		m_pipeline->draw(operation->vertex_count, operation->instance_count, operation->first_vertex, operation->first_instance);
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
	inv_view = &ctx.camera->get_inv_view();
	view_translation = math::fvec4(ctx.camera->get_translation());
	view_rotation = math::fmat4(math::fmat3(*view));
	projection = &ctx.camera->get_projection();
	view_projection = &ctx.camera->get_view_projection();
	camera_position = &ctx.camera->get_translation();
	camera_exposure = ctx.camera->get_exposure_normalization();
}

void material_pass::evaluate_lighting(const render::context& ctx, std::uint32_t layer_mask)
{
	// Reset light and shadow counts
	light_probe_count = 0;
	directional_light_count = 0;
	directional_shadow_count = 0;
	spot_light_count = 0;
	point_light_count = 0;
	rectangle_light_count = 0;
	
	const auto& light_probes = ctx.collection->get_objects(scene::light_probe::object_type_id);
	for (const scene::object_base* object: light_probes)
	{
		if (!(object->get_layer_mask() & layer_mask))
		{
			continue;
		}
		
		if (!light_probe_count)
		{
			const scene::light_probe& light_probe = static_cast<const scene::light_probe&>(*object);
			++light_probe_count;
			light_probe_luminance_texture = light_probe.get_luminance_texture().get();
			light_probe_illuminance_texture = light_probe.get_illuminance_texture().get();
		}
	}
	
	const auto& lights = ctx.collection->get_objects(scene::light::object_type_id);
	for (const scene::object_base* object: lights)
	{
		if (!(object->get_layer_mask() & layer_mask))
		{
			continue;
		}
		
		const scene::light& light = static_cast<const scene::light&>(*object);
		
		switch (light.get_light_type())
		{
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
				
				directional_light_colors[index] = directional_light.get_colored_illuminance() * ctx.camera->get_exposure_normalization();
				directional_light_directions[index] = directional_light.get_direction() * ctx.camera->get_rotation();
				
				// Add directional shadow
				if (directional_light.is_shadow_caster() && directional_light.get_shadow_framebuffer())
				{
					const std::size_t index = directional_shadow_count;
					
					++directional_shadow_count;
					if (directional_shadow_count > directional_shadow_maps.size())
					{
						directional_shadow_maps.resize(directional_shadow_count);
						directional_shadow_splits.resize(directional_shadow_count);
						directional_shadow_fade_ranges.resize(directional_shadow_count);
						directional_shadow_matrices.resize(directional_shadow_count);
					}
					
					directional_shadow_maps[index] = directional_light.get_shadow_texture().get();
					directional_shadow_splits[index] = directional_light.get_shadow_cascade_distances();
					directional_shadow_fade_ranges[index] = directional_light.get_shadow_fade_range();
					directional_shadow_matrices[index] = directional_light.get_shadow_cascade_matrices();
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
				spot_light_positions[index] = spot_light.get_translation() - ctx.camera->get_translation();
				spot_light_directions[index] = spot_light.get_direction() * ctx.camera->get_rotation();
				spot_light_cutoffs[index] = spot_light.get_cosine_cutoff();
				break;
			}
			
			// Add point light
			case scene::light_type::point:
			{
				const scene::point_light& point_light = static_cast<const scene::point_light&>(light);
				
				const std::size_t index = point_light_count;
				
				++point_light_count;
				if (point_light_count > point_light_colors.size())
				{
					point_light_colors.resize(point_light_count);
					point_light_positions.resize(point_light_count);
				}
				
				point_light_colors[index] = point_light.get_colored_luminous_flux() * ctx.camera->get_exposure_normalization();
				point_light_positions[index] = point_light.get_translation() - ctx.camera->get_translation();
				
				break;
			}
			
			// Add rectangle light
			case scene::light_type::rectangle:
			{
				const scene::rectangle_light& rectangle_light = static_cast<const scene::rectangle_light&>(light);
				
				const std::size_t index = rectangle_light_count;
				
				++rectangle_light_count;
				if (rectangle_light_count > rectangle_light_colors.size())
				{
					rectangle_light_colors.resize(rectangle_light_count);
					rectangle_light_corners.resize(rectangle_light_count * 4);
				}
				
				rectangle_light_colors[index] = rectangle_light.get_colored_luminance() * ctx.camera->get_exposure_normalization();
				
				const auto corners = rectangle_light.get_corners();
				for (std::size_t i = 0; i < 4; ++i)
				{
					rectangle_light_corners[index * 4 + i] = (corners[i] - ctx.camera->get_translation()) * ctx.camera->get_rotation();
				}
				
				break;
			}
			
			default:
				break;
		}
	}
	
	// Generate lighting state hash
	lighting_state_hash = std::hash<std::size_t>{}(light_probe_count);
	lighting_state_hash = hash_combine(lighting_state_hash, std::hash<std::size_t>{}(directional_light_count));
	lighting_state_hash = hash_combine(lighting_state_hash, std::hash<std::size_t>{}(directional_shadow_count));
	lighting_state_hash = hash_combine(lighting_state_hash, std::hash<std::size_t>{}(point_light_count));
	lighting_state_hash = hash_combine(lighting_state_hash, std::hash<std::size_t>{}(spot_light_count));
	lighting_state_hash = hash_combine(lighting_state_hash, std::hash<std::size_t>{}(rectangle_light_count));
}

void material_pass::evaluate_misc(const render::context& ctx)
{
	time = ctx.t;
	timestep = ctx.dt;
	subframe = ctx.alpha;
	
	const auto& viewport_dimensions = (m_framebuffer) ? m_framebuffer->dimensions() : m_pipeline->get_default_framebuffer_dimensions();
	resolution = 
	{
		static_cast<float>(viewport_dimensions[0]),
		static_cast<float>(viewport_dimensions[1])
	};
	
	///mouse_position = ...
}

std::unique_ptr<gl::shader_program> material_pass::generate_shader_program(const gl::shader_template& shader_template, material_blend_mode blend_mode) const
{
	std::unordered_map<std::string, std::string> definitions;
	
	definitions["VERTEX_POSITION"]    = std::to_string(vertex_attribute_location::position);
	definitions["VERTEX_UV"]          = std::to_string(vertex_attribute_location::uv);
	definitions["VERTEX_NORMAL"]      = std::to_string(vertex_attribute_location::normal);
	definitions["VERTEX_TANGENT"]     = std::to_string(vertex_attribute_location::tangent);
	definitions["VERTEX_COLOR"]       = std::to_string(vertex_attribute_location::color);
	definitions["VERTEX_BONE_INDEX"]  = std::to_string(vertex_attribute_location::bone_index);
	definitions["VERTEX_BONE_WEIGHT"] = std::to_string(vertex_attribute_location::bone_weight);
	definitions["VERTEX_BARYCENTRIC"] = std::to_string(vertex_attribute_location::barycentric);
	definitions["VERTEX_TARGET"]      = std::to_string(vertex_attribute_location::target);
	
	definitions["FRAGMENT_OUTPUT_COLOR"] = "0";
	
	definitions["LIGHT_PROBE_COUNT"] = std::to_string(light_probe_count);
	definitions["DIRECTIONAL_LIGHT_COUNT"] = std::to_string(directional_light_count);
	definitions["DIRECTIONAL_SHADOW_COUNT"] = std::to_string(directional_shadow_count);
	definitions["POINT_LIGHT_COUNT"] = std::to_string(point_light_count);
	definitions["SPOT_LIGHT_COUNT"] = std::to_string(spot_light_count);
	definitions["RECTANGLE_LIGHT_COUNT"] = std::to_string(rectangle_light_count);
	
	if (blend_mode == material_blend_mode::masked)
	{
		definitions["MASKED_OPACITY"] = "1";
	}
	
	auto shader_program = shader_template.build(definitions);
	
	if (!shader_program->linked())
	{
		debug::log_error("Failed to link material shader program: {}", shader_program->info());
		debug::log_warning("{}", shader_template.configure(gl::shader_stage::fragment, definitions));
	}
	
	return shader_program;
}

void material_pass::build_shader_command_buffer(std::vector<std::function<void()>>& command_buffer, const gl::shader_program& shader_program) const
{
	// Bind shader program
	command_buffer.emplace_back([&](){m_pipeline->bind_shader_program(&shader_program);});
	
	// Update camera variables
	if (auto view_var = shader_program.variable("view"))
	{
		command_buffer.emplace_back([&, view_var](){view_var->update(*view);});
	}
	if (auto inv_view_var = shader_program.variable("inv_view"))
	{
		command_buffer.emplace_back([&, inv_view_var](){inv_view_var->update(*inv_view);});
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
	
	// Update IBL variables
	if (auto brdf_lut_var = shader_program.variable("brdf_lut"))
	{
		command_buffer.emplace_back
		(
			[&, brdf_lut_var]()
			{
				brdf_lut_var->update(*brdf_lut);
			}
		);
	}
	
	// Update light probe variables
	if (light_probe_count)
	{
		if (auto light_probe_luminance_texture_var = shader_program.variable("light_probe_luminance_texture"))
		{
			command_buffer.emplace_back
			(
				[&, light_probe_luminance_texture_var]()
				{
					light_probe_luminance_texture_var->update(*light_probe_luminance_texture);
				}
			);
		}
		
		if (auto light_probe_luminance_mip_scale_var = shader_program.variable("light_probe_luminance_mip_scale"))
		{
			command_buffer.emplace_back
			(
				[&, light_probe_luminance_mip_scale_var]()
				{
					light_probe_luminance_mip_scale_var->update(std::max<float>(static_cast<float>(light_probe_luminance_texture->get_image_view()->get_mip_level_count()) - 4.0f, 0.0f));
				}
			);
		}
		
		if (auto light_probe_illuminance_texture_var = shader_program.variable("light_probe_illuminance_texture"))
		{
			command_buffer.emplace_back
			(
				[&, light_probe_illuminance_texture_var]()
				{
					light_probe_illuminance_texture_var->update(*light_probe_illuminance_texture);
				}
			);
		}
	}
	
	// Update LTC variables
	if (auto ltc_lut_1_var = shader_program.variable("ltc_lut_1"))
	{
		if (auto ltc_lut_2_var = shader_program.variable("ltc_lut_2"))
		{
			command_buffer.emplace_back
			(
				[&, ltc_lut_1_var, ltc_lut_2_var]()
				{
					ltc_lut_1_var->update(*ltc_lut_1);
					ltc_lut_2_var->update(*ltc_lut_2);
				}
			);
		}
	}
	if (rectangle_light_count)
	{
		if (auto rectangle_light_colors_var = shader_program.variable("rectangle_light_colors"))
		{
			auto rectangle_light_corners_var = shader_program.variable("rectangle_light_corners");
			
			if (rectangle_light_corners_var)
			{
				command_buffer.emplace_back
				(
					[&, rectangle_light_colors_var, rectangle_light_corners_var]()
					{
						rectangle_light_colors_var->update(std::span<const math::fvec3>{rectangle_light_colors.data(), rectangle_light_count});
						rectangle_light_corners_var->update(std::span<const math::fvec3>{rectangle_light_corners.data(), rectangle_light_count * 4});
					}
				);
			}
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
						directional_light_colors_var->update(std::span<const math::fvec3>{directional_light_colors.data(), directional_light_count});
						directional_light_directions_var->update(std::span<const math::fvec3>{directional_light_directions.data(), directional_light_count});
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
			auto directional_shadow_splits_var = shader_program.variable("directional_shadow_splits");
			auto directional_shadow_fade_ranges_var = shader_program.variable("directional_shadow_fade_ranges");
			auto directional_shadow_matrices_var = shader_program.variable("directional_shadow_matrices");
			
			if (directional_shadow_maps_var && directional_shadow_splits_var && directional_shadow_fade_ranges_var && directional_shadow_matrices_var)
			{
				command_buffer.emplace_back
				(
					[&, directional_shadow_maps_var, directional_shadow_splits_var, directional_shadow_fade_ranges_var, directional_shadow_matrices_var]()
					{
						directional_shadow_maps_var->update(std::span<const gl::texture_2d* const>{directional_shadow_maps.data(), directional_shadow_count});
						
						std::size_t offset = 0;
						for (std::size_t i = 0; i < directional_shadow_count; ++i)
						{
							directional_shadow_splits_var->update(directional_shadow_splits[i], i);
							directional_shadow_fade_ranges_var->update(directional_shadow_fade_ranges[i], i);
							directional_shadow_matrices_var->update(directional_shadow_matrices[i], offset);
							offset += directional_shadow_matrices[i].size();
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
						point_light_colors_var->update(std::span<const math::fvec3>{point_light_colors.data(), point_light_count});
						point_light_positions_var->update(std::span<const math::fvec3>{point_light_positions.data(), point_light_count});
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
						spot_light_colors_var->update(std::span<const math::fvec3>{spot_light_colors.data(), spot_light_count});
						spot_light_positions_var->update(std::span<const math::fvec3>{spot_light_positions.data(), spot_light_count});
						spot_light_directions_var->update(std::span<const math::fvec3>{spot_light_directions.data(), spot_light_count});
						spot_light_cutoffs_var->update(std::span<const math::fvec2>{spot_light_cutoffs.data(), spot_light_count});
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
				normal_model_var->update(math::transpose(math::inverse(math::fmat3(*model))));
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
				model_view_var->update(model_view);
				normal_model_view_var->update(math::transpose(math::inverse(math::fmat3(model_view))));
			}
		);
	}
	else
	{
		if (model_view_var)
		{
			command_buffer.emplace_back([&, model_view_var](){model_view_var->update(model_view);});
		}
		else if (normal_model_view_var)
		{
			command_buffer.emplace_back
			(
				[&, normal_model_view_var]()
				{
					normal_model_view_var->update(math::transpose(math::inverse(math::fmat3(model_view))));
				}
			);
		}
	}
	
	// Update model-view-projection matrix variable
	if (auto model_view_projection_var = shader_program.variable("model_view_projection"))
	{
		command_buffer.emplace_back([&, model_view_projection_var](){model_view_projection_var->update((*projection) * model_view);});
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
			/// @TODO render::matvar_bool is broken due to the std::vector<bool> specialization.
			// case gl::shader_variable_type::bvec1:
				// if (material_var->type() == material_variable_type::bvec1)
				// {
					// command_buffer.emplace_back
					// (
						// [size, shader_var, material_var = std::static_pointer_cast<matvar_bool>(material_var)]()
						// {
							// shader_var->update(std::span<const bool>{material_var->data(), size});
						// }
					// );
				// }
				// break;
			case gl::shader_variable_type::bvec2:
				if (material_var->type() == material_variable_type::bvec2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_bvec2>(material_var)]()
						{
							shader_var->update(std::span<const math::bvec2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::bvec3:
				if (material_var->type() == material_variable_type::bvec3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_bvec3>(material_var)]()
						{
							shader_var->update(std::span<const math::bvec3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::bvec4:
				if (material_var->type() == material_variable_type::bvec4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_bvec4>(material_var)]()
						{
							shader_var->update(std::span<const math::bvec4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::ivec1:
				if (material_var->type() == material_variable_type::ivec1)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_int>(material_var)]()
						{
							shader_var->update(std::span<const int>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::ivec2:
				if (material_var->type() == material_variable_type::ivec2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_ivec2>(material_var)]()
						{
							shader_var->update(std::span<const math::ivec2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::ivec3:
				if (material_var->type() == material_variable_type::ivec3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_ivec3>(material_var)]()
						{
							shader_var->update(std::span<const math::ivec3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::ivec4:
				if (material_var->type() == material_variable_type::ivec4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_ivec4>(material_var)]()
						{
							shader_var->update(std::span<const math::ivec4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uvec1:
				if (material_var->type() == material_variable_type::uvec1)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_uint>(material_var)]()
						{
							shader_var->update(std::span<const unsigned int>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uvec2:
				if (material_var->type() == material_variable_type::uvec2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_uvec2>(material_var)]()
						{
							shader_var->update(std::span<const math::uvec2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uvec3:
				if (material_var->type() == material_variable_type::uvec3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_uvec3>(material_var)]()
						{
							shader_var->update(std::span<const math::uvec3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::uvec4:
				if (material_var->type() == material_variable_type::uvec4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_uvec4>(material_var)]()
						{
							shader_var->update(std::span<const math::uvec4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fvec1:
				if (material_var->type() == material_variable_type::fvec1)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_float>(material_var)]()
						{
							shader_var->update(std::span<const float>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fvec2:
				if (material_var->type() == material_variable_type::fvec2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_fvec2>(material_var)]()
						{
							shader_var->update(std::span<const math::fvec2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fvec3:
				if (material_var->type() == material_variable_type::fvec3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_fvec3>(material_var)]()
						{
							shader_var->update(std::span<const math::fvec3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fvec4:
				if (material_var->type() == material_variable_type::fvec4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_fvec4>(material_var)]()
						{
							shader_var->update(std::span<const math::fvec4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fmat2:
				if (material_var->type() == material_variable_type::fmat2)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_fmat2>(material_var)]()
						{
							shader_var->update(std::span<const math::fmat2>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fmat3:
				if (material_var->type() == material_variable_type::fmat3)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_fmat3>(material_var)]()
						{
							shader_var->update(std::span<const math::fmat3>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::fmat4:
				if (material_var->type() == material_variable_type::fmat4)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_fmat4>(material_var)]()
						{
							shader_var->update(std::span<const math::fmat4>{material_var->data(), size});
						}
					);
				}
				break;
			case gl::shader_variable_type::texture_1d:
				if (material_var->type() == material_variable_type::texture_1d)
				{
					command_buffer.emplace_back
					(
						[size, shader_var, material_var = std::static_pointer_cast<matvar_texture_1d>(material_var)]()
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
						[size, shader_var, material_var = std::static_pointer_cast<matvar_texture_2d>(material_var)]()
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
						[size, shader_var, material_var = std::static_pointer_cast<matvar_texture_3d>(material_var)]()
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
						[size, shader_var, material_var = std::static_pointer_cast<matvar_texture_cube>(material_var)]()
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

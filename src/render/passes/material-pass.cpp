/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "render/passes/material-pass.hpp"
#include "config.hpp"
#include "resources/resource-manager.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "render/vertex-attribute.hpp"
#include "render/material-flags.hpp"
#include "render/model.hpp"
#include "render/context.hpp"
#include "scene/camera.hpp"
#include "scene/collection.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/point-light.hpp"
#include "scene/spot-light.hpp"
#include "config.hpp"
#include "math/quaternion.hpp"
#include "math/projection.hpp"
#include <cmath>
#include <glad/glad.h>

#include "render/passes/shadow-map-pass.hpp"

namespace render {

static bool operation_compare(const render::operation& a, const render::operation& b);

material_pass::material_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	fallback_material(nullptr),
	mouse_position({0.0f, 0.0f}),
	shadow_map_pass(nullptr),
	shadow_map(nullptr)
{
	max_ambient_light_count = MATERIAL_PASS_MAX_AMBIENT_LIGHT_COUNT;
	max_point_light_count = MATERIAL_PASS_MAX_POINT_LIGHT_COUNT;
	max_directional_light_count = MATERIAL_PASS_MAX_DIRECTIONAL_LIGHT_COUNT;
	max_spot_light_count = MATERIAL_PASS_MAX_SPOTLIGHT_COUNT;
	
	ambient_light_colors = new float3[max_ambient_light_count];
	point_light_colors = new float3[max_point_light_count];
	point_light_positions = new float3[max_point_light_count];
	point_light_attenuations = new float3[max_point_light_count];
	directional_light_colors = new float3[max_directional_light_count];
	directional_light_directions = new float3[max_directional_light_count];
	directional_light_textures = new const gl::texture_2d*[max_directional_light_count];
	directional_light_texture_matrices = new float4x4[max_directional_light_count];
	directional_light_texture_opacities = new float[max_directional_light_count];
	
	spot_light_colors = new float3[max_spot_light_count];
	spot_light_positions = new float3[max_spot_light_count];
	spot_light_directions = new float3[max_spot_light_count];
	spot_light_attenuations = new float3[max_spot_light_count];
	spot_light_cutoffs = new float2[max_spot_light_count];
}

material_pass::~material_pass()
{
	delete[] ambient_light_colors;
	delete[] point_light_colors;
	delete[] point_light_positions;
	delete[] point_light_attenuations;
	delete[] directional_light_colors;
	delete[] directional_light_directions;
	delete[] directional_light_textures;
	delete[] directional_light_texture_matrices;
	delete[] directional_light_texture_opacities;
	delete[] spot_light_colors;
	delete[] spot_light_positions;
	delete[] spot_light_directions;
	delete[] spot_light_attenuations;
	delete[] spot_light_cutoffs;
}

void material_pass::render(const render::context& ctx, render::queue& queue) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_GREATER);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0x00);
	
	// For half-z buffer
	glDepthRange(-1.0f, 1.0f);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const float3& camera_position = ctx.camera_transform.translation;
	const float4x4& view = ctx.view;
	const float4x4& projection = ctx.projection;
	const float4x4& view_projection = ctx.view_projection;
	float4x4 model_view_projection;
	float4x4 model;
	float4x4 model_view;
	float3x3 normal_model;
	float3x3 normal_model_view;
	float2 clip_depth;
	clip_depth[0] = ctx.camera->get_clip_near_tween().interpolate(ctx.alpha);
	clip_depth[1] = ctx.camera->get_clip_far_tween().interpolate(ctx.alpha);
	float log_depth_coef = 2.0f / std::log2(clip_depth[1] + 1.0f);

	int active_material_flags = 0;
	const gl::shader_program* active_shader_program = nullptr;
	const render::material* active_material = nullptr;
	const parameter_set* parameters = nullptr;
	
	// Reset light counts
	ambient_light_count = 0;
	point_light_count = 0;
	directional_light_count = 0;
	spot_light_count = 0;
	
	// Collect lights
	const std::list<scene::object_base*>* lights = ctx.collection->get_objects(scene::light::object_type_id);
	for (const scene::object_base* object: *lights)
	{
		// Skip inactive lights
		if (!object->is_active())
				continue;
		
		const scene::light* light = static_cast<const scene::light*>(object);
		switch (light->get_light_type())
		{
			// Add ambient light
			case scene::light_type::ambient:
			{
				if (ambient_light_count < max_ambient_light_count)
				{
					// Pre-expose light
					ambient_light_colors[ambient_light_count] = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
					++ambient_light_count;
				}
				break;
			}
			
			// Add point light
			case scene::light_type::point:
			{
				if (point_light_count < max_point_light_count)
				{
					// Pre-expose light
					point_light_colors[point_light_count] = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
					
					float3 position = light->get_transform_tween().interpolate(ctx.alpha).translation;
					point_light_positions[point_light_count] = position;
					
					point_light_attenuations[point_light_count] = static_cast<const scene::point_light*>(light)->get_attenuation_tween().interpolate(ctx.alpha);
					++point_light_count;
				}
				break;
			}
			
			// Add directional light
			case scene::light_type::directional:
			{
				if (directional_light_count < max_directional_light_count)
				{
					const scene::directional_light* directional_light = static_cast<const scene::directional_light*>(light);

					// Pre-expose light
					directional_light_colors[directional_light_count] = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
					
					float3 direction = static_cast<const scene::directional_light*>(light)->get_direction_tween().interpolate(ctx.alpha);
					directional_light_directions[directional_light_count] = direction;
					
					
					if (directional_light->get_light_texture())
					{
						directional_light_textures[directional_light_count] = directional_light->get_light_texture();
						directional_light_texture_opacities[directional_light_count] = directional_light->get_light_texture_opacity_tween().interpolate(ctx.alpha);
						
						math::transform<float> light_transform = light->get_transform_tween().interpolate(ctx.alpha);
						float3 forward = light_transform.rotation * config::global_forward;
						float3 up = light_transform.rotation * config::global_up;
						float4x4 light_view = math::look_at(light_transform.translation, light_transform.translation + forward, up);
						
						float2 scale = directional_light->get_light_texture_scale_tween().interpolate(ctx.alpha);
						float4x4 light_projection = math::ortho(-scale.x(), scale.x(), -scale.y(), scale.y(), -1.0f, 1.0f);
						
						directional_light_texture_matrices[directional_light_count] = light_projection * light_view;
					}
					else
					{
						directional_light_textures[directional_light_count] = nullptr;
						directional_light_texture_opacities[directional_light_count] = 0.0f;
					}
					
					++directional_light_count;
				}
				break;
			}
			
			// Add spot_light
			case scene::light_type::spot:
			{
				if (spot_light_count < max_spot_light_count)
				{
					const scene::spot_light* spot_light = static_cast<const scene::spot_light*>(light);

					// Pre-expose light
					spot_light_colors[spot_light_count] = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
					
					float3 position = light->get_transform_tween().interpolate(ctx.alpha).translation;
					spot_light_positions[spot_light_count] = position;
					
					float3 direction = spot_light->get_direction_tween().interpolate(ctx.alpha);
					spot_light_directions[spot_light_count] = direction;
					
					spot_light_attenuations[spot_light_count] = spot_light->get_attenuation_tween().interpolate(ctx.alpha);
					spot_light_cutoffs[spot_light_count] = spot_light->get_cosine_cutoff_tween().interpolate(ctx.alpha);
					
					++spot_light_count;
				}
				break;
			}

			default:
				break;
		}
	}
	
	float4x4 shadow_matrices_directional[4];
	float4 shadow_splits_directional;
	
	if (shadow_map_pass)
	{
		for (int i = 0; i < 4; ++i)
			shadow_matrices_directional[i] = shadow_map_pass->get_shadow_matrices()[i];

		// Calculate shadow map split distances
		for (int i = 0; i < 4; ++i)
			shadow_splits_directional[i] = shadow_map_pass->get_split_distances()[i + 1];
	}
	
	// Sort render queue
	queue.sort(operation_compare);

	for (const render::operation& operation: queue)
	{
		// Get operation material
		const render::material* material = operation.material;
		if (!material)
		{
			if (fallback_material)
			{
				// No material specified, use fallback material
				material = fallback_material;
			}
			else
			{
				// No material specified and no fallback material, skip operation
				continue;
			}
		}
		
		// Switch materials if necessary
		if (active_material != material)
		{
			active_material = material;
			
			// Change rasterizer state according to material flags
			std::uint32_t material_flags = active_material->get_flags();
			if (active_material_flags != material_flags)
			{
				if ((material_flags & MATERIAL_FLAG_TRANSLUCENT) != (active_material_flags & MATERIAL_FLAG_TRANSLUCENT))
				{
					if (material_flags & MATERIAL_FLAG_TRANSLUCENT)
					{
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					}
					else
					{
						glDisable(GL_BLEND);
					}
				}

				if ((material_flags & MATERIAL_FLAG_BACK_FACES) != (active_material_flags & MATERIAL_FLAG_BACK_FACES))
				{
					if (material_flags & MATERIAL_FLAG_BACK_FACES)
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_FRONT);
					}
					else
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					}
				}
				else if ((material_flags & MATERIAL_FLAG_FRONT_AND_BACK_FACES) != (active_material_flags & MATERIAL_FLAG_FRONT_AND_BACK_FACES))
				{
					if (material_flags & MATERIAL_FLAG_FRONT_AND_BACK_FACES)
					{
						glDisable(GL_CULL_FACE);
					}
					else
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GL_BACK);
					}
				}

				if ((material_flags & MATERIAL_FLAG_X_RAY) != (active_material_flags & MATERIAL_FLAG_X_RAY))
				{
					if (material_flags & MATERIAL_FLAG_X_RAY)
					{
						glDisable(GL_DEPTH_TEST);

					}
					else
					{
						glEnable(GL_DEPTH_TEST);
					}
				}
				
				if ((material_flags & MATERIAL_FLAG_DECAL_SURFACE) != (active_material_flags & MATERIAL_FLAG_DECAL_SURFACE))
				{
					if (material_flags & MATERIAL_FLAG_DECAL_SURFACE)
					{
						glEnable(GL_STENCIL_TEST);
						glStencilFunc(GL_ALWAYS, 1, ~0);
						glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
						glStencilMask(~0);
					}
					else
					{
						glDisable(GL_STENCIL_TEST);
						glStencilMask(0);
					}
				}
				
				if ((material_flags & MATERIAL_FLAG_DECAL) != (active_material_flags & MATERIAL_FLAG_DECAL))
				{
					if (material_flags & MATERIAL_FLAG_DECAL)
					{
						glEnable(GL_DEPTH_TEST);
						glDepthFunc(GL_GEQUAL);
						glDepthMask(GL_FALSE);
						
						glEnable(GL_STENCIL_TEST);
						glStencilFunc(GL_EQUAL, 1, ~0);
						//glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
						//glStencilMask(~0);
						glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
						glStencilMask(0);
					}
					else
					{
						glEnable(GL_DEPTH_TEST);
						glDepthFunc(GL_GREATER);
						glDepthMask(GL_TRUE);
						glDisable(GL_STENCIL_TEST);
						glStencilMask(0);
					}
				}
				
				/*
				if ((material_flags & MATERIAL_FLAG_OUTLINE) != (active_material_flags & MATERIAL_FLAG_OUTLINE))
				{
					if (material_flags & MATERIAL_FLAG_OUTLINE)
					{
						glEnable(GL_STENCIL_TEST);
						glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
						glStencilFunc(GL_ALWAYS, 2, 0xFF);
						glStencilMask(0xFF);
					}
					else
					{
						glDisable(GL_STENCIL_TEST);
						glStencilMask(0x00);
					}
				}
				*/

				active_material_flags = material_flags;
			}
			
			// Switch shaders if necessary
			const gl::shader_program* shader_program = active_material->get_shader_program();
			if (active_shader_program != shader_program)
			{
				active_shader_program = shader_program;
				if (!active_shader_program)
				{
					continue;
				}
				
				// Change shader program
				rasterizer->use_program(*active_shader_program);

				// Get set of known shader input parameters
				if (auto it = parameter_sets.find(active_shader_program); it != parameter_sets.end())
				{
					parameters = it->second;
				}
				else
				{
					parameters = load_parameter_set(active_shader_program);
				}

				// Upload context-dependent shader parameters
				if (parameters->time)
					parameters->time->upload(ctx.t);
				if (parameters->mouse)
					parameters->mouse->upload(mouse_position);
				if (parameters->resolution)
					parameters->resolution->upload(resolution);
				if (parameters->camera_position)
					parameters->camera_position->upload(camera_position);
				if (parameters->camera_exposure)
					parameters->camera_exposure->upload(ctx.exposure);
				if (parameters->view)
					parameters->view->upload(view);
				if (parameters->view_projection)
					parameters->view_projection->upload(view_projection);
				if (parameters->ambient_light_count)
					parameters->ambient_light_count->upload(ambient_light_count);
				if (parameters->ambient_light_colors)
					parameters->ambient_light_colors->upload(0, ambient_light_colors, ambient_light_count);
				if (parameters->point_light_count)
					parameters->point_light_count->upload(point_light_count);
				if (parameters->point_light_colors)
					parameters->point_light_colors->upload(0, point_light_colors, point_light_count);
				if (parameters->point_light_positions)
					parameters->point_light_positions->upload(0, point_light_positions, point_light_count);
				if (parameters->point_light_attenuations)
					parameters->point_light_attenuations->upload(0, point_light_attenuations, point_light_count);
				if (parameters->directional_light_count)
					parameters->directional_light_count->upload(directional_light_count);
				if (parameters->directional_light_colors)
					parameters->directional_light_colors->upload(0, directional_light_colors, directional_light_count);
				if (parameters->directional_light_directions)
					parameters->directional_light_directions->upload(0, directional_light_directions, directional_light_count);
				
				if (parameters->directional_light_textures)
					parameters->directional_light_textures->upload(0, directional_light_textures, directional_light_count);
				if (parameters->directional_light_texture_matrices)
					parameters->directional_light_texture_matrices->upload(0, directional_light_texture_matrices, directional_light_count);
				if (parameters->directional_light_texture_opacities)
					parameters->directional_light_texture_opacities->upload(0, directional_light_texture_opacities, directional_light_count);
				
				if (parameters->spot_light_count)
					parameters->spot_light_count->upload(spot_light_count);
				if (parameters->spot_light_colors)
					parameters->spot_light_colors->upload(0, spot_light_colors, spot_light_count);
				if (parameters->spot_light_positions)
					parameters->spot_light_positions->upload(0, spot_light_positions, spot_light_count);
				if (parameters->spot_light_directions)
					parameters->spot_light_directions->upload(0, spot_light_directions, spot_light_count);
				if (parameters->spot_light_attenuations)
					parameters->spot_light_attenuations->upload(0, spot_light_attenuations, spot_light_count);
				if (parameters->spot_light_cutoffs)
					parameters->spot_light_cutoffs->upload(0, spot_light_cutoffs, spot_light_count);
				
				if (parameters->shadow_map_directional && shadow_map)
					parameters->shadow_map_directional->upload(shadow_map);
				if (parameters->shadow_matrices_directional)
					parameters->shadow_matrices_directional->upload(0, shadow_matrices_directional, 4);
				if (parameters->shadow_splits_directional)
					parameters->shadow_splits_directional->upload(shadow_splits_directional);
			}
			
			// Upload material properties to shader
			active_material->upload(ctx.alpha);
		}

		// Calculate operation-dependent parameters
		model = operation.transform;
		model_view_projection = view_projection * model;
		model_view = view * model;
		normal_model = math::transpose(math::inverse(math::matrix<float, 3, 3>(model)));
		normal_model_view = math::transpose(math::inverse(math::matrix<float, 3, 3>(model_view)));
		
		// Skinning palette
		if (operation.bone_count && parameters->skinning_palette)
		{
			parameters->skinning_palette->upload(0, operation.skinning_palette, operation.bone_count);
		}

		// Upload operation-dependent parameters
		if (parameters->model)
			parameters->model->upload(model);
		if (parameters->model_view)
			parameters->model_view->upload(model_view);
		if (parameters->model_view_projection)
			parameters->model_view_projection->upload(model_view_projection);
		if (parameters->normal_model)
			parameters->normal_model->upload(normal_model);
		if (parameters->normal_model_view)
			parameters->normal_model_view->upload(normal_model_view);
		if (parameters->clip_depth)
			parameters->clip_depth->upload(clip_depth);
		if (parameters->log_depth_coef)
			parameters->log_depth_coef->upload(log_depth_coef);

		// Draw geometry
		if (operation.instance_count)
			rasterizer->draw_arrays_instanced(*operation.vertex_array, operation.drawing_mode, operation.start_index, operation.index_count, operation.instance_count);
		else
			rasterizer->draw_arrays(*operation.vertex_array, operation.drawing_mode, operation.start_index, operation.index_count);
	}
}

void material_pass::set_fallback_material(const material* fallback)
{
	this->fallback_material = fallback;
}

const material_pass::parameter_set* material_pass::load_parameter_set(const gl::shader_program* program) const
{
	// Allocate a new parameter set
	parameter_set* parameters = new parameter_set();

	// Connect inputs
	parameters->time = program->get_input("time");
	parameters->mouse = program->get_input("mouse");
	parameters->resolution = program->get_input("resolution");
	parameters->camera_position = program->get_input("camera.position");
	parameters->camera_exposure = program->get_input("camera.exposure");
	parameters->model = program->get_input("model");
	parameters->view = program->get_input("view");
	parameters->projection = program->get_input("projection");
	parameters->model_view = program->get_input("model_view");
	parameters->view_projection = program->get_input("view_projection");
	parameters->model_view_projection = program->get_input("model_view_projection");
	parameters->normal_model = program->get_input("normal_model");
	parameters->normal_model_view = program->get_input("normal_model_view");
	parameters->clip_depth = program->get_input("clip_depth");
	parameters->log_depth_coef = program->get_input("log_depth_coef");
	parameters->ambient_light_count = program->get_input("ambient_light_count");
	parameters->ambient_light_colors = program->get_input("ambient_light_colors");
	parameters->point_light_count = program->get_input("point_light_count");
	parameters->point_light_colors = program->get_input("point_light_colors");
	parameters->point_light_positions = program->get_input("point_light_positions");
	parameters->point_light_attenuations = program->get_input("point_light_attenuations");
	parameters->directional_light_count = program->get_input("directional_light_count");
	parameters->directional_light_colors = program->get_input("directional_light_colors");
	parameters->directional_light_directions = program->get_input("directional_light_directions");
	parameters->directional_light_textures = program->get_input("directional_light_textures");
	parameters->directional_light_texture_matrices = program->get_input("directional_light_texture_matrices");
	parameters->directional_light_texture_opacities = program->get_input("directional_light_texture_opacities");
	parameters->spot_light_count = program->get_input("spot_light_count");
	parameters->spot_light_colors = program->get_input("spot_light_colors");
	parameters->spot_light_positions = program->get_input("spot_light_positions");
	parameters->spot_light_directions = program->get_input("spot_light_directions");
	parameters->spot_light_attenuations = program->get_input("spot_light_attenuations");
	parameters->spot_light_cutoffs = program->get_input("spot_light_cutoffs");
	parameters->shadow_map_directional = program->get_input("shadow_map_directional");
	parameters->shadow_splits_directional = program->get_input("shadow_splits_directional");
	parameters->shadow_matrices_directional = program->get_input("shadow_matrices_directional");
	parameters->skinning_palette = program->get_input("skinning_palette");

	// Add parameter set to map of parameter sets
	parameter_sets[program] = parameters;

	return parameters;
}

void material_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}

bool operation_compare(const render::operation& a, const render::operation& b)
{
	if (!a.material)
		return false;
	else if (!b.material)
		return true;
	
	bool xray_a = a.material->get_flags() & MATERIAL_FLAG_X_RAY;
	bool xray_b = b.material->get_flags() & MATERIAL_FLAG_X_RAY;
	
	if (xray_a)
	{
		if (xray_b)
		{
			// A and B are both xray, render back to front
			return (a.depth >= b.depth);
		}
		else
		{
			// A is xray, B is not. Render B first
			return false;
		}
	}
	else
	{
		if (xray_b)
		{
			// A is opaque, B is xray. Render A first
			return true;
		}
		else
		{
			// Determine transparency
			bool transparent_a = a.material->get_flags() & MATERIAL_FLAG_TRANSLUCENT;
			bool transparent_b = b.material->get_flags() & MATERIAL_FLAG_TRANSLUCENT;
			
			if (transparent_a)
			{
				if (transparent_b)
				{
					// Determine decal status
					bool decal_a = a.material->get_flags() & MATERIAL_FLAG_DECAL;
					bool decal_b = b.material->get_flags() & MATERIAL_FLAG_DECAL;
					
					if (decal_a)
					{
						if (decal_b)
						{
							// A and B are both transparent decals, render back to front
							return (a.depth >= b.depth);
						}
						else
						{
							// A is a transparent decal, B is transparent but not a decal, render A first
							return true;
						}
					}
					else
					{
						if (decal_b)
						{
							// A is transparent but not a decal, B is a transparent decal, render B first
							return false;
						}
						else
						{
							// A and B are both transparent, but not decals, render back to front
							return (a.depth <= b.depth);
						}
					}
				}
				else
				{
					// A is transparent, B is opaque. Render B first
					return false;
				}
			}
			else
			{
				if (transparent_b)
				{
					// A is opaque, B is transparent. Render A first
					return true;
				}
				else
				{
					// A and B are both opaque
					if (a.material->get_shader_program() == b.material->get_shader_program())
					{
						// A and B have the same shader
						if (a.vertex_array == b.vertex_array)
						{
							// A and B have the same VAO, render front to back
							return (a.depth < b.depth);
						}
						else
						{
							// Sort by VAO
							return (a.vertex_array < b.vertex_array);
						}
					}
					else
					{
						// A and B are both opaque and have different shaders, sort by shader
						return (a.material->get_shader_program() < b.material->get_shader_program());
					}
				}
			}
		}
	}
}

} // namespace render

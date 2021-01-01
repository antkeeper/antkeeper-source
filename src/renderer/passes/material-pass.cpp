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

#include "renderer/passes/material-pass.hpp"
#include "configuration.hpp"
#include "resources/resource-manager.hpp"
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
#include "rasterizer/shader.hpp"
#include "rasterizer/shader-type.hpp"
#include "rasterizer/shader-program.hpp"
#include "rasterizer/shader-input.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "rasterizer/vertex-array.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "rasterizer/drawing-mode.hpp"
#include "rasterizer/texture-2d.hpp"
#include "rasterizer/texture-wrapping.hpp"
#include "rasterizer/texture-filter.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/material-flags.hpp"
#include "renderer/model.hpp"
#include "renderer/render-context.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/point-light.hpp"
#include "scene/spotlight.hpp"
#include "scene/scene.hpp"
#include "configuration.hpp"
#include "math/math.hpp"
#include <cmath>
#include <glad/glad.h>

#include "shadow-map-pass.hpp"

static bool operation_compare(const render_operation& a, const render_operation& b);

material_pass::material_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	fallback_material(nullptr),
	time_tween(nullptr),
	mouse_position({0.0f, 0.0f}),
	focal_point_tween(nullptr),
	shadow_map_pass(nullptr),
	shadow_map(nullptr),
	shadow_strength(1.0f)
{
	soft_shadows_texture = resource_manager->load<texture_2d>("tree-shadow.png");
	soft_shadows_texture->set_wrapping(texture_wrapping::clamp, texture_wrapping::clamp);
	soft_shadows_texture->set_filters(texture_min_filter::linear_mipmap_linear, texture_mag_filter::linear);
	
	max_ambient_light_count = MATERIAL_PASS_MAX_AMBIENT_LIGHT_COUNT;
	max_point_light_count = MATERIAL_PASS_MAX_POINT_LIGHT_COUNT;
	max_directional_light_count = MATERIAL_PASS_MAX_DIRECTIONAL_LIGHT_COUNT;
	max_spotlight_count = MATERIAL_PASS_MAX_SPOTLIGHT_COUNT;
	
	ambient_light_colors = new float3[max_ambient_light_count];
	point_light_colors = new float3[max_point_light_count];
	point_light_positions = new float3[max_point_light_count];
	point_light_attenuations = new float3[max_point_light_count];
	directional_light_colors = new float3[max_directional_light_count];
	directional_light_directions = new float3[max_directional_light_count];
	spotlight_colors = new float3[max_spotlight_count];
	spotlight_positions = new float3[max_spotlight_count];
	spotlight_directions = new float3[max_spotlight_count];
	spotlight_attenuations = new float3[max_spotlight_count];
	spotlight_cutoffs = new float2[max_spotlight_count];
}

material_pass::~material_pass()
{
	delete[] ambient_light_colors;
	delete[] point_light_colors;
	delete[] point_light_positions;
	delete[] point_light_attenuations;
	delete[] directional_light_colors;
	delete[] directional_light_directions;
	delete[] spotlight_colors;
	delete[] spotlight_positions;
	delete[] spotlight_directions;
	delete[] spotlight_attenuations;
	delete[] spotlight_cutoffs;
}

void material_pass::render(render_context* context) const
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
	
	float time = (time_tween) ? time_tween->interpolate(context->alpha) : 0.0f;
	float3 focal_point = (focal_point_tween) ? focal_point_tween->interpolate(context->alpha) : float3{0, 0, 0};
	float4x4 view = context->camera->get_view_tween().interpolate(context->alpha);
	float4x4 projection = context->camera->get_projection_tween().interpolate(context->alpha);
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection;
	float4x4 model;
	float4x4 model_view;
	float3x3 normal_model_view;
	float2 clip_depth;
	clip_depth[0] = context->camera->get_clip_near_tween().interpolate(context->alpha);
	clip_depth[1] = context->camera->get_clip_far_tween().interpolate(context->alpha);
	float log_depth_coef = 2.0f / std::log2(clip_depth[1] + 1.0f);


	int active_material_flags = 0;
	const ::shader_program* active_shader_program = nullptr;
	const ::material* active_material = nullptr;
	const parameter_set* parameters = nullptr;
	
	// Reset light counts
	ambient_light_count = 0;
	point_light_count = 0;
	directional_light_count = 0;
	spotlight_count = 0;
	
	// Collect lights
	const std::list<scene_object_base*>* lights = context->scene->get_objects(light::object_type_id);
	for (const scene_object_base* object: *lights)
	{
		// Skip inactive lights
		if (!object->is_active())
				continue;
		
		const ::light* light = static_cast<const ::light*>(object);
		switch (light->get_light_type())
		{
			// Add ambient light
			case light_type::ambient:
			{
				if (ambient_light_count < max_ambient_light_count)
				{
					ambient_light_colors[ambient_light_count] = light->get_scaled_color_tween().interpolate(context->alpha);
					++ambient_light_count;
				}
				break;
			}
			
			// Add point light
			case light_type::point:
			{
				if (point_light_count < max_point_light_count)
				{
					point_light_colors[point_light_count] = light->get_scaled_color_tween().interpolate(context->alpha);
					
					// Transform position into view-space
					float3 position = light->get_transform_tween().interpolate(context->alpha).translation;
					float3 view_space_position = math::resize<3>(view * float4{position.x, position.y, position.z, 1.0f});
					point_light_positions[point_light_count] = view_space_position;
					
					point_light_attenuations[point_light_count] = static_cast<const point_light*>(light)->get_attenuation_tween().interpolate(context->alpha);
					++point_light_count;
				}
				break;
			}
			
			// Add directional light
			case light_type::directional:
			{
				if (directional_light_count < max_directional_light_count)
				{
					directional_light_colors[directional_light_count] = light->get_scaled_color_tween().interpolate(context->alpha);
					
					// Transform direction into view-space
					float3 direction = static_cast<const directional_light*>(light)->get_direction_tween().interpolate(context->alpha);
					float3 view_space_direction = math::normalize(math::resize<3>(view * math::resize<4>(-direction)));
					directional_light_directions[directional_light_count] = view_space_direction;
					
					++directional_light_count;
				}
				break;
			}
			
			// Add spotlight
			case light_type::spot:
			{
				if (spotlight_count < max_spotlight_count)
				{
					spotlight_colors[spotlight_count] = light->get_scaled_color_tween().interpolate(context->alpha);
					
					// Transform position into view-space
					float3 position = light->get_transform_tween().interpolate(context->alpha).translation;
					float3 view_space_position = math::resize<3>(view * float4{position.x, position.y, position.z, 1.0f});
					spotlight_positions[spotlight_count] = view_space_position;
					
					const ::spotlight* spotlight = static_cast<const ::spotlight*>(light);
					
					// Transform direction into view-space
					float3 direction = spotlight->get_direction_tween().interpolate(context->alpha);
					float3 view_space_direction = math::normalize(math::resize<3>(view * float4{-direction.x, -direction.y, -direction.z, 0.0f}));
					spotlight_directions[spotlight_count] = view_space_direction;
					
					spotlight_attenuations[spotlight_count] = spotlight->get_attenuation_tween().interpolate(context->alpha);
					spotlight_cutoffs[spotlight_count] = spotlight->get_cosine_cutoff_tween().interpolate(context->alpha);
					
					++spotlight_count;
				}
				break;
			}

			default:
				break;
		}
	}
	
	float4x4 shadow_map_matrices[4];
	float4 shadow_map_split_distances;
	
	if (shadow_map_pass)
	{
		for (int i = 0; i < 4; ++i)
			shadow_map_matrices[i] = shadow_map_pass->get_shadow_matrices()[i];

		// Calculate shadow map split distances
		for (int i = 0; i < 4; ++i)
			shadow_map_split_distances[i] = shadow_map_pass->get_split_distances()[i + 1];
	}

	// Sort render operations
	context->operations.sort(operation_compare);

	for (const render_operation& operation: context->operations)
	{
		// Get operation material
		const ::material* material = operation.material;
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
			const ::shader_program* shader_program = active_material->get_shader_program();
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
					parameters->time->upload(time);
				if (parameters->mouse)
					parameters->mouse->upload(mouse_position);
				if (parameters->resolution)
					parameters->resolution->upload(resolution);
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
				if (parameters->spotlight_count)
					parameters->spotlight_count->upload(spotlight_count);
				if (parameters->spotlight_colors)
					parameters->spotlight_colors->upload(0, spotlight_colors, spotlight_count);
				if (parameters->spotlight_positions)
					parameters->spotlight_positions->upload(0, spotlight_positions, spotlight_count);
				if (parameters->spotlight_directions)
					parameters->spotlight_directions->upload(0, spotlight_directions, spotlight_count);
				if (parameters->spotlight_attenuations)
					parameters->spotlight_attenuations->upload(0, spotlight_attenuations, spotlight_count);
				if (parameters->spotlight_cutoffs)
					parameters->spotlight_cutoffs->upload(0, spotlight_cutoffs, spotlight_count);
				if (parameters->soft_shadows)
					parameters->soft_shadows->upload(soft_shadows_texture);
				if (parameters->focal_point)
					parameters->focal_point->upload(focal_point);
				if (parameters->shadow_map_matrices)
					parameters->shadow_map_matrices->upload(0, shadow_map_matrices, 4);
				if (parameters->shadow_map_split_distances)
					parameters->shadow_map_split_distances->upload(shadow_map_split_distances);
				if (parameters->shadow_map && shadow_map)
					parameters->shadow_map->upload(shadow_map);
				if (parameters->shadow_strength)
					parameters->shadow_strength->upload(shadow_strength);
			}
			
			// Upload material properties to shader
			active_material->upload(context->alpha);
		}

		// Calculate operation-dependent parameters
		model = operation.transform;
		model_view_projection = view_projection * model;
		model_view = view * model;
		normal_model_view = math::transpose(math::inverse(math::resize<3, 3>(model_view)));

		// Upload operation-dependent parameters
		if (parameters->model)
			parameters->model->upload(model);
		if (parameters->model_view)
			parameters->model_view->upload(model_view);
		if (parameters->model_view_projection)
			parameters->model_view_projection->upload(model_view_projection);
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

void material_pass::set_time_tween(const tween<double>* time)
{
	this->time_tween = time;
}

void material_pass::set_shadow_strength(float strength)
{
	this->shadow_strength = strength;
}

void material_pass::set_focal_point_tween(const tween<float3>* focal_point)
{
	this->focal_point_tween = focal_point;
}

const material_pass::parameter_set* material_pass::load_parameter_set(const shader_program* program) const
{
	// Allocate a new parameter set
	parameter_set* parameters = new parameter_set();

	// Connect inputs
	parameters->time = program->get_input("time");
	parameters->mouse = program->get_input("mouse");
	parameters->resolution = program->get_input("resolution");
	parameters->model = program->get_input("model");
	parameters->view = program->get_input("view");
	parameters->projection = program->get_input("projection");
	parameters->model_view = program->get_input("model_view");
	parameters->view_projection = program->get_input("view_projection");
	parameters->model_view_projection = program->get_input("model_view_projection");
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
	parameters->spotlight_count = program->get_input("spotlight_count");
	parameters->spotlight_colors = program->get_input("spotlight_colors");
	parameters->spotlight_positions = program->get_input("spotlight_positions");
	parameters->spotlight_directions = program->get_input("spotlight_directions");
	parameters->spotlight_attenuations = program->get_input("spotlight_attenuations");
	parameters->spotlight_cutoffs = program->get_input("spotlight_cutoffs");
	parameters->soft_shadows = program->get_input("soft_shadows");
	parameters->focal_point = program->get_input("focal_point");
	parameters->shadow_map_matrices = program->get_input("shadow_map_matrices");
	parameters->shadow_map_split_distances = program->get_input("shadow_map_split_distances");
	parameters->shadow_map = program->get_input("shadow_map");
	parameters->shadow_strength = program->get_input("shadow_strength");

	// Add parameter set to map of parameter sets
	parameter_sets[program] = parameters;

	return parameters;
}

void material_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}

bool operation_compare(const render_operation& a, const render_operation& b)
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
							return (a.depth >= b.depth);
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


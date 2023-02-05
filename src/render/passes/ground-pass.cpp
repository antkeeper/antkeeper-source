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

#include "render/passes/ground-pass.hpp"
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
#include "render/context.hpp"
#include "render/model.hpp"
#include "render/material.hpp"
#include "scene/camera.hpp"
#include "scene/collection.hpp"
#include "scene/directional-light.hpp"
#include "scene/ambient-light.hpp"
#include "utility/fundamental-types.hpp"
#include "color/color.hpp"
#include "math/interpolation.hpp"
#include <cmath>
#include <stdexcept>
#include <glad/glad.h>

namespace render {

ground_pass::ground_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	ground_model(nullptr),
	ground_model_vao(nullptr),
	ground_material(nullptr),
	shader_program(nullptr)
{}

ground_pass::~ground_pass()
{}

void ground_pass::render(const render::context& ctx, render::queue& queue) const
{
	if (!ground_model)
		return;
	
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_TRUE);
	glDepthRange(-1.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const scene::camera& camera = *ctx.camera;
	float clip_near = camera.get_clip_near_tween().interpolate(ctx.alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(ctx.alpha);
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	float4x4 model = math::scale(math::matrix4<float>::identity(), model_scale);
	float4x4 view = float4x4(float3x3(ctx.view));
	float4x4 model_view = view * model;
	const float4x4& projection = ctx.projection;
	const float4x4& view_projection = ctx.view_projection;
	float4x4 model_view_projection = projection * model_view;
	
	
	
	float3 ambient_light_color = {0.0f, 0.0f, 0.0f};
	float3 directional_light_color = {0.0f, 0.0f, 0.0f};
	float3 directional_light_direction = {0.0f, 0.0f, 0.0f};
	
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
				// Pre-expose light
				ambient_light_color = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
				break;
			}
			
			// Add directional light
			case scene::light_type::directional:
			{
				const scene::directional_light* directional_light = static_cast<const scene::directional_light*>(light);

				// Pre-expose light
				float3 light_color = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
				if (light_color.x() < directional_light_color.x())
					break;
				
				directional_light_color = light_color;
				
				directional_light_direction = static_cast<const scene::directional_light*>(light)->get_direction_tween().interpolate(ctx.alpha);
				break;
			}

			default:
				break;
		}
	}
	
	// Draw ground
	rasterizer->use_program(*shader_program);
	
	if (model_view_projection_input)
		model_view_projection_input->upload(model_view_projection);
	if (view_projection_input)
		view_projection_input->upload(view_projection);
	if (camera_position_input)
		camera_position_input->upload(ctx.camera_transform.translation);
	if (directional_light_colors_input)
		directional_light_colors_input->upload(0, &directional_light_color, 1);
	if (directional_light_directions_input)
		directional_light_directions_input->upload(0, &directional_light_direction, 1);
	if (ambient_light_colors_input)
		ambient_light_colors_input->upload(0, &ambient_light_color, 1);
	
	ground_material->upload(ctx.alpha);
	
	

	rasterizer->draw_arrays(*ground_model_vao, ground_model_drawing_mode, ground_model_start_index, ground_model_index_count);
}

void ground_pass::set_ground_model(const model* model)
{
	ground_model = model;
	
	if (ground_model)
	{
		ground_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			ground_material = group->get_material();
			ground_model_drawing_mode = group->get_drawing_mode();
			ground_model_start_index = group->get_start_index();
			ground_model_index_count = group->get_index_count();
		}
		
		if (ground_material)
		{
			shader_program = ground_material->get_shader_program();
			
			if (shader_program)
			{
				model_view_projection_input = shader_program->get_input("model_view_projection");
				view_projection_input = shader_program->get_input("view_projection");
				camera_position_input = shader_program->get_input("camera.position");
				directional_light_colors_input = shader_program->get_input("directional_light_colors");
				directional_light_directions_input = shader_program->get_input("directional_light_directions");
				ambient_light_colors_input = shader_program->get_input("ambient_light_colors");
			}
		}
	}
	else
	{
		ground_model_vao = nullptr;
	}
}

} // namespace render

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

#include <engine/render/passes/ground-pass.hpp>
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
#include <engine/render/vertex-attribute.hpp>
#include <engine/render/context.hpp>
#include <engine/render/model.hpp>
#include <engine/render/material.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/math/vector.hpp>
#include <engine/color/color.hpp>
#include <engine/math/interpolation.hpp>
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

void ground_pass::render(render::context& ctx)
{
	/*
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
	
	math::fvec2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const scene::camera& camera = *ctx.camera;
	float clip_near = camera.get_clip_near_tween().interpolate(ctx.alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(ctx.alpha);
	math::fvec3 model_scale = math::fvec3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	math::fmat4 model = math::scale(math::fmat4::identity(), model_scale);
	math::fmat4 view = math::fmat4(math::fmat3(ctx.view));
	math::fmat4 model_view = view * model;
	const math::fmat4& projection = ctx.projection;
	const math::fmat4& view_projection = ctx.view_projection;
	math::fmat4 model_view_projection = projection * model_view;
	
	math::fvec3 directional_light_color = {0.0f, 0.0f, 0.0f};
	math::fvec3 directional_light_direction = {0.0f, 0.0f, 0.0f};
	
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
			// Add directional light
			case scene::light_type::directional:
			{
				const scene::directional_light* directional_light = static_cast<const scene::directional_light*>(light);

				// Pre-expose light
				math::fvec3 light_color = light->get_scaled_color_tween().interpolate(ctx.alpha) * ctx.exposure;
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
	
	if (model_view_projection_var)
		model_view_projection_var->update(model_view_projection);
	if (view_projection_var)
		view_projection_var->update(view_projection);
	if (camera_position_var)
		camera_position_var->update(ctx.camera_transform.translation);
	if (directional_light_colors_var)
		directional_light_colors_var->update(0, &directional_light_color, 1);
	if (directional_light_directions_var)
		directional_light_directions_var->update(0, &directional_light_direction, 1);
	
	ground_material->update(ctx.alpha);

	rasterizer->draw_arrays(*ground_model_vao, ground_model_drawing_mode, ground_model_start_index, ground_model_index_count);
	*/
}

void ground_pass::set_ground_model(std::shared_ptr<render::model> model)
{
	/*
	ground_model = model;
	
	if (ground_model)
	{
		ground_model_vao = model->get_vertex_array().get();
		
		for (const auto& group: model->get_groups())
		{
			ground_material = group.material;
			ground_model_drawing_mode = group.drawing_mode;
			ground_model_start_index = group.start_index;
			ground_model_index_count = group.index_count;
		}
		
		if (ground_material)
		{
			shader_program = ground_material->get_shader_program();
			
			if (shader_program)
			{
				model_view_projection_var = shader_program->get_var("model_view_projection");
				view_projection_var = shader_program->get_var("view_projection");
				camera_position_var = shader_program->get_var("camera.position");
				directional_light_colors_var = shader_program->get_var("directional_light_colors");
				directional_light_directions_var = shader_program->get_var("directional_light_directions");
			}
		}
	}
	else
	{
		ground_model_vao = nullptr;
	}
	*/
}

} // namespace render

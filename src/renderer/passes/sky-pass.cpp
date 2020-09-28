/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "renderer/passes/sky-pass.hpp"
#include "resources/resource-manager.hpp"
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
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
#include "renderer/render-context.hpp"
#include "renderer/model.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "scene/scene.hpp"
#include "utility/fundamental-types.hpp"
#include <cmath>
#include <glad/glad.h>

sky_pass::sky_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	mouse_position({0.0f, 0.0f}),
	time_of_day(0.0f),
	sky_model(nullptr),
	sky_model_vao(nullptr),
	blue_noise_map(nullptr),
	observer_coordinates{0.0f, 0.0f}
{
	shader_program = resource_manager->load<::shader_program>("sky.glsl");
	model_view_projection_input = shader_program->get_input("model_view_projection");
	sun_color_input = shader_program->get_input("sun_color");
	sky_gradient_input = shader_program->get_input("sky_gradient");
	mouse_input = shader_program->get_input("mouse");
	resolution_input = shader_program->get_input("resolution");
	time_input = shader_program->get_input("time");
	time_of_day_input = shader_program->get_input("time_of_day");
	blue_noise_map_input = shader_program->get_input("blue_noise_map");
	observer_coordinates_input = shader_program->get_input("observer_coordinates");
	sun_position_input = shader_program->get_input("sun_position");
	sun_az_el_input = shader_program->get_input("sun_az_el");
	moon_position_input = shader_program->get_input("moon_position");
	moon_az_el_input = shader_program->get_input("moon_az_el");
	
	const float vertex_data[] =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};

	std::size_t vertex_size = 3;
	std::size_t vertex_stride = sizeof(float) * vertex_size;
	std::size_t vertex_count = 6;

	quad_vbo = new vertex_buffer(sizeof(float) * vertex_size * vertex_count, vertex_data);
	quad_vao = new vertex_array();
	quad_vao->bind_attribute(VERTEX_POSITION_LOCATION, *quad_vbo, 3, vertex_attribute_type::float_32, vertex_stride, 0);
	
	sky_gradient[0] = {1.0, 0.0f, 0.0f, 0.0f};
	sky_gradient[1] = {0.0, 1.0f, 0.0f, 0.333f};
	sky_gradient[2] = {0.0, 0.0f, 1.0f, 0.667f};
	sky_gradient[3] = {1.0, 1.0f, 0.0f, 1.0f};
}

sky_pass::~sky_pass()
{
	delete quad_vao;
	delete quad_vbo;
}

void sky_pass::render(render_context* context) const
{
	if (!sky_model_vao)
		return;
	
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float time = (time_tween) ? time_tween->interpolate(context->alpha) : 0.0f;
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const ::camera& camera = *context->camera;
	float clip_near = camera.get_clip_near_tween().interpolate(context->alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(context->alpha);
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	float4x4 model = math::scale(math::identity4x4<float>, model_scale);
	float4x4 model_view = math::resize<4, 4>(math::resize<3, 3>(camera.get_view_tween().interpolate(context->alpha))) * model;
	float4x4 projection = camera.get_projection_tween().interpolate(context->alpha);
	float4x4 model_view_projection = projection * model_view;
	
	// Change shader program
	rasterizer->use_program(*shader_program);

	// Upload shader parameters
	if (model_view_projection_input)
		model_view_projection_input->upload(model_view_projection);
	if (sun_color_input)
		sun_color_input->upload(sun_color);
	if (sky_gradient_input)
		sky_gradient_input->upload(0, &sky_gradient[0], 4);
	if (mouse_input)
		mouse_input->upload(mouse_position);
	if (resolution_input)
		resolution_input->upload(resolution);
	if (time_input)
		time_input->upload(time);
	if (time_of_day_input)
		time_of_day_input->upload(time_of_day);
	if (blue_noise_map_input)
		blue_noise_map_input->upload(blue_noise_map);
	if (observer_coordinates_input)
		observer_coordinates_input->upload(observer_coordinates);
	
	if (sun_position_input)
		sun_position_input->upload(sun_position);
	if (sun_az_el_input)
		sun_az_el_input->upload(sun_az_el);
	if (moon_position_input)
		moon_position_input->upload(moon_position);
	if (moon_az_el_input)
		moon_az_el_input->upload(moon_az_el);
	
	// Draw sky model
	rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
}

void sky_pass::set_sky_model(const model* model)
{
	sky_model = model;
	
	if (sky_model)
	{
		sky_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			sky_model_drawing_mode = group->get_drawing_mode();
			sky_model_start_index = group->get_start_index();
			sky_model_index_count = group->get_index_count();
		}
	}
	else
	{
		sky_model_vao = nullptr;
	}
}

void sky_pass::set_sun_color(const float3& color)
{
	sun_color = color;
}

void sky_pass::set_sky_gradient(const std::array<float4, 4>& gradient)
{
	sky_gradient = gradient;
}

void sky_pass::set_time_of_day(float time)
{
	time_of_day = time;
}

void sky_pass::set_time_tween(const tween<double>* time)
{
	this->time_tween = time;
}

void sky_pass::set_blue_noise_map(const texture_2d* texture)
{
	blue_noise_map = texture;
}

void sky_pass::set_observer_coordinates(const float2& coordinates)
{
	observer_coordinates = coordinates;
}

void sky_pass::set_sun_coordinates(const float3& position, const float2& az_el)
{
	sun_position = position;
	sun_az_el = az_el;
}

void sky_pass::set_moon_coordinates(const float3& position, const float2& az_el)
{
	moon_position = position;
	moon_az_el = az_el;
}

void sky_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}

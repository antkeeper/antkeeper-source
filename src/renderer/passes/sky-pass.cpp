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

#include "renderer/passes/sky-pass.hpp"
#include "resources/resource-manager.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute-type.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/render-context.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "scene/camera.hpp"
#include "utility/fundamental-types.hpp"
#include "math/interpolation.hpp"
#include <cmath>
#include <glad/glad.h>

sky_pass::sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	mouse_position({0.0f, 0.0f}),
	sky_model(nullptr),
	sky_material(nullptr),
	sky_model_vao(nullptr),
	sky_shader_program(nullptr),
	moon_model(nullptr),
	moon_material(nullptr),
	moon_model_vao(nullptr),
	moon_shader_program(nullptr),
	blue_noise_map(nullptr),
	observer_location{0.0f, 0.0f, 0.0f},
	time_tween(nullptr),
	time_of_day_tween(0.0, math::lerp<float, float>),
	julian_day_tween(0.0, math::lerp<float, float>),
	sun_position_tween(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>),
	sun_az_el_tween(float2{0.0f, 0.0f}, math::lerp<float2, float>),
	moon_position_tween(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>),
	moon_az_el_tween(float2{0.0f, 0.0f}, math::lerp<float2, float>),
	horizon_color_tween(float3{0.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	zenith_color_tween(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>)
{}

sky_pass::~sky_pass()
{}

void sky_pass::render(render_context* context) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float time = time_tween->interpolate(context->alpha);
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const scene::camera& camera = *context->camera;
	float clip_near = camera.get_clip_near_tween().interpolate(context->alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(context->alpha);
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	float4x4 model = math::scale(math::identity4x4<float>, model_scale);
	float4x4 view = math::resize<4, 4>(math::resize<3, 3>(camera.get_view_tween().interpolate(context->alpha)));
	float4x4 model_view = view * model;
	float4x4 projection = camera.get_projection_tween().interpolate(context->alpha);
	float4x4 model_view_projection = projection * model_view;
	
	float time_of_day = time_of_day_tween.interpolate(context->alpha);
	float julian_day = julian_day_tween.interpolate(context->alpha);
	float3 sun_position = sun_position_tween.interpolate(context->alpha);
	float2 sun_az_el = sun_az_el_tween.interpolate(context->alpha);
	float3 moon_position = moon_position_tween.interpolate(context->alpha);
	float2 moon_az_el = moon_az_el_tween.interpolate(context->alpha);
	float3 horizon_color = horizon_color_tween.interpolate(context->alpha);
	float3 zenith_color = zenith_color_tween.interpolate(context->alpha);
	
	// Draw sky model
	{
		rasterizer->use_program(*sky_shader_program);

		// Upload shader parameters
		if (model_view_projection_input)
			model_view_projection_input->upload(model_view_projection);
		if (horizon_color_input)
			horizon_color_input->upload(horizon_color);
		if (zenith_color_input)
			zenith_color_input->upload(zenith_color);
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
		if (observer_location_input)
			observer_location_input->upload(observer_location);
		if (sun_position_input)
			sun_position_input->upload(sun_position);
		if (sun_az_el_input)
			sun_az_el_input->upload(sun_az_el);
		if (moon_position_input)
			moon_position_input->upload(moon_position);
		if (moon_az_el_input)
			moon_az_el_input->upload(moon_az_el);
		if (julian_day_input)
			julian_day_input->upload(julian_day);
		if (cos_moon_angular_radius_input)
			cos_moon_angular_radius_input->upload(cos_moon_angular_radius);
		if (cos_sun_angular_radius_input)
			cos_sun_angular_radius_input->upload(cos_sun_angular_radius);
		
		sky_material->upload(context->alpha);

		rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
	}
	
	// Draw moon model
	if (moon_az_el[1] >= -moon_angular_radius)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		
		float moon_distance = (clip_near + clip_far) * 0.5f;		
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = moon_position * -moon_distance;
		moon_transform.rotation = moon_rotation;
		moon_transform.scale = {moon_radius, moon_radius, moon_radius};
		
		model = math::matrix_cast(moon_transform);		
		model_view = view * model;
		model_view_projection = projection * model_view;
		float3x3 normal_model = math::transpose(math::inverse(math::resize<3, 3>(model)));
		
		rasterizer->use_program(*moon_shader_program);
		if (moon_model_view_projection_input)
			moon_model_view_projection_input->upload(model_view_projection);
		if (moon_normal_model_input)
			moon_normal_model_input->upload(normal_model);
		if (moon_moon_position_input)
			moon_moon_position_input->upload(moon_position);
		if (moon_sun_position_input)
			moon_sun_position_input->upload(sun_position);
		moon_material->upload(context->alpha);
		rasterizer->draw_arrays(*moon_model_vao, moon_model_drawing_mode, moon_model_start_index, moon_model_index_count);
	}
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
			sky_material = group->get_material();
			sky_model_drawing_mode = group->get_drawing_mode();
			sky_model_start_index = group->get_start_index();
			sky_model_index_count = group->get_index_count();
		}
		
		if (sky_material)
		{
			sky_shader_program = sky_material->get_shader_program();
			
			if (sky_shader_program)
			{
				model_view_projection_input = sky_shader_program->get_input("model_view_projection");
				horizon_color_input = sky_shader_program->get_input("horizon_color");
				zenith_color_input = sky_shader_program->get_input("zenith_color");
				mouse_input = sky_shader_program->get_input("mouse");
				resolution_input = sky_shader_program->get_input("resolution");
				time_input = sky_shader_program->get_input("time");
				time_of_day_input = sky_shader_program->get_input("time_of_day");
				blue_noise_map_input = sky_shader_program->get_input("blue_noise_map");
				observer_location_input = sky_shader_program->get_input("observer_location");
				sun_position_input = sky_shader_program->get_input("sun_position");
				sun_az_el_input = sky_shader_program->get_input("sun_az_el");
				moon_position_input = sky_shader_program->get_input("moon_position");
				moon_az_el_input = sky_shader_program->get_input("moon_az_el");
				julian_day_input = sky_shader_program->get_input("julian_day");
				cos_moon_angular_radius_input = sky_shader_program->get_input("cos_moon_angular_radius");
				cos_sun_angular_radius_input = sky_shader_program->get_input("cos_sun_angular_radius");
			}
		}
	}
	else
	{
		sky_model_vao = nullptr;
	}
}

void sky_pass::set_moon_model(const model* model)
{
	moon_model = model;
	
	if (moon_model)
	{
		moon_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			moon_material = group->get_material();
			moon_model_drawing_mode = group->get_drawing_mode();
			moon_model_start_index = group->get_start_index();
			moon_model_index_count = group->get_index_count();
		}
		
		if (moon_material)
		{
			moon_shader_program = moon_material->get_shader_program();
			
			if (moon_shader_program)
			{
				moon_model_view_projection_input = moon_shader_program->get_input("model_view_projection");
				moon_normal_model_input = moon_shader_program->get_input("normal_model");
				moon_moon_position_input = moon_shader_program->get_input("moon_position");
				moon_sun_position_input = moon_shader_program->get_input("sun_position");
			}
		}
	}
	else
	{
		moon_model = nullptr;
	}
}

void sky_pass::update_tweens()
{
	julian_day_tween.update();
	sun_position_tween.update();
	sun_az_el_tween.update();
	moon_position_tween.update();
	moon_az_el_tween.update();
	time_of_day_tween.update();
	horizon_color_tween.update();
	zenith_color_tween.update();
}

void sky_pass::set_time_of_day(float time)
{
	time_of_day_tween[1] = time;
}

void sky_pass::set_time_tween(const tween<double>* time)
{
	this->time_tween = time;
}

void sky_pass::set_blue_noise_map(const gl::texture_2d* texture)
{
	blue_noise_map = texture;
}

void sky_pass::set_julian_day(float jd)
{
	julian_day_tween[1] = jd;
}

void sky_pass::set_observer_location(float altitude, float latitude, float longitude)
{
	observer_location = {altitude, latitude, longitude};
}

void sky_pass::set_sun_coordinates(const float3& position, const float2& az_el)
{
	sun_position_tween[1] = position;
	sun_az_el_tween[1] = az_el;
}

void sky_pass::set_moon_coordinates(const float3& position, const float2& az_el)
{
	moon_position_tween[1] = position;
	moon_az_el_tween[1] = az_el;
}

void sky_pass::set_moon_rotation(const math::quaternion<float>& rotation)
{
	moon_rotation = rotation;
}

void sky_pass::set_moon_angular_radius(float radius)
{
	moon_angular_radius = radius;
	cos_moon_angular_radius = std::cos(moon_angular_radius);
}

void sky_pass::set_sun_angular_radius(float radius)
{
	sun_angular_radius = radius;
	cos_sun_angular_radius = std::cos(sun_angular_radius);
}

void sky_pass::set_horizon_color(const float3& color)
{
	horizon_color_tween[1] = color;
}

void sky_pass::set_zenith_color(const float3& color)
{
	zenith_color_tween[1] = color;
}

void sky_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}

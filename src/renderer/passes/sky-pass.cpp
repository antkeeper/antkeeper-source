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
#include "resources/string-table.hpp"
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
#include "renderer/vertex-attribute.hpp"
#include "renderer/context.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "scene/camera.hpp"
#include "utility/fundamental-types.hpp"
#include "color/color.hpp"
#include "astro/illuminance.hpp"
#include "math/interpolation.hpp"
#include "geom/cartesian.hpp"
#include "geom/spherical.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/light/photometry.hpp"
#include <cmath>
#include <stdexcept>
#include <glad/glad.h>
#include <iostream>

sky_pass::sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	mouse_position({0.0f, 0.0f}),
	sky_model(nullptr),
	sky_material(nullptr),
	sky_model_vao(nullptr),
	sky_shader_program(nullptr),
	moon_model(nullptr),
	moon_model_vao(nullptr),
	moon_material(nullptr),
	moon_shader_program(nullptr),
	stars_model(nullptr),
	stars_model_vao(nullptr),
	star_material(nullptr),
	star_shader_program(nullptr),
	clouds_model(nullptr),
	clouds_model_vao(nullptr),
	cloud_material(nullptr),
	cloud_shader_program(nullptr),
	observer_altitude_tween(0.0f, math::lerp<float, float>),
	sun_position_tween(float3{1.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	sun_color_outer_tween(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>),
	sun_color_inner_tween(float3{1.0f, 1.0f, 1.0f}, math::lerp<float3, float>),
	topocentric_frame_translation({0, 0, 0}, math::lerp<float3, float>),
	topocentric_frame_rotation(math::quaternion<float>::identity(), math::nlerp<float>)
{}

sky_pass::~sky_pass()
{}

void sky_pass::render(const render::context& ctx, render::queue& queue) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	const scene::camera& camera = *ctx.camera;
	float clip_near = camera.get_clip_near_tween().interpolate(ctx.alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(ctx.alpha);
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	float4x4 model = math::scale(math::identity4x4<float>, model_scale);
	float4x4 view = math::resize<4, 4>(math::resize<3, 3>(camera.get_view_tween().interpolate(ctx.alpha)));
	float4x4 model_view = view * model;
	float4x4 projection = camera.get_projection_tween().interpolate(ctx.alpha);
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection = projection * model_view;
	float exposure = std::exp2(camera.get_exposure_tween().interpolate(ctx.alpha));
	
	// Interpolate observer altitude
	float observer_altitude = observer_altitude_tween.interpolate(ctx.alpha);
	
	// Construct tweened inertial to topocentric frame
	physics::frame<float> topocentric_frame =
	{
		topocentric_frame_translation.interpolate(ctx.alpha),
		topocentric_frame_rotation.interpolate(ctx.alpha)
	};
	
	// Get topocentric space direction to sun
	float3 sun_position = sun_position_tween.interpolate(ctx.alpha);
	float3 sun_direction = math::normalize(sun_position);
	
	// Interpolate sun color
	float3 sun_color_outer = sun_color_outer_tween.interpolate(ctx.alpha);
	float3 sun_color_inner = sun_color_inner_tween.interpolate(ctx.alpha);
	
	// Draw atmosphere
	if (sky_model)
	{
		rasterizer->use_program(*sky_shader_program);

		// Upload shader parameters
		if (model_view_projection_input)
			model_view_projection_input->upload(model_view_projection);
		if (mouse_input)
			mouse_input->upload(mouse_position);
		if (resolution_input)
			resolution_input->upload(resolution);
		if (time_input)
			time_input->upload(ctx.t);
		if (exposure_input)
			exposure_input->upload(exposure);
		
		if (observer_altitude_input)
			observer_altitude_input->upload(observer_altitude);
		if (sun_direction_input)
			sun_direction_input->upload(sun_direction);
		if (sun_angular_radius_input)
			sun_angular_radius_input->upload(sun_angular_radius);
		if (sun_color_input)
			sun_color_input->upload(sun_color_outer);
		if (scale_height_rm_input)
			scale_height_rm_input->upload(scale_height_rm);
		if (rayleigh_scattering_input)
			rayleigh_scattering_input->upload(rayleigh_scattering);
		if (mie_scattering_input)
			mie_scattering_input->upload(mie_scattering);
		if (mie_anisotropy_input)
			mie_anisotropy_input->upload(mie_anisotropy);
		if (atmosphere_radii_input)
			atmosphere_radii_input->upload(atmosphere_radii);
		
		sky_material->upload(ctx.alpha);

		rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
	}
	
	// Draw clouds
	if (clouds_model)
	{
		rasterizer->use_program(*cloud_shader_program);
		
		if (cloud_model_view_projection_input)
			cloud_model_view_projection_input->upload(model_view_projection);
		if (cloud_sun_direction_input)
			cloud_sun_direction_input->upload(sun_direction);
		if (cloud_sun_color_input)
			cloud_sun_color_input->upload(sun_color_inner);
		if (cloud_camera_position_input)
			cloud_camera_position_input->upload(ctx.camera_transform.translation);
		if (cloud_camera_exposure_input)
			cloud_camera_exposure_input->upload(exposure);
		
		cloud_material->upload(ctx.alpha);

		rasterizer->draw_arrays(*clouds_model_vao, clouds_model_drawing_mode, clouds_model_start_index, clouds_model_index_count);
	}
	
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_ONE, GL_ONE);
	
	// Draw stars
	if (stars_model)
	{
		float star_distance = (clip_near + clip_far) * 0.5f;
		
		model = math::resize<4, 4>(math::matrix_cast<float>(topocentric_frame.rotation));
		model = math::scale(model, {star_distance, star_distance, star_distance});
		
		model_view = view * model;
		
		rasterizer->use_program(*star_shader_program);
		if (star_model_view_input)
			star_model_view_input->upload(model_view);
		if (star_projection_input)
			star_projection_input->upload(projection);
		if (star_distance_input)
			star_distance_input->upload(star_distance);
		if (star_exposure_input)
			star_exposure_input->upload(exposure);
		
		star_material->upload(ctx.alpha);
		
		rasterizer->draw_arrays(*stars_model_vao, stars_model_drawing_mode, stars_model_start_index, stars_model_index_count);
	}
	
	// Draw moon model
	/*
	float3 moon_position = {0, 0, 0};
	if (moon_position.y >= -moon_angular_radius)
	{

		
		float moon_distance = (clip_near + clip_far) * 0.5f;		
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = moon_position * -moon_distance;
		moon_transform.rotation = math::quaternion<float>::identity();
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
		moon_material->upload(ctx.alpha);
		rasterizer->draw_arrays(*moon_model_vao, moon_model_drawing_mode, moon_model_start_index, moon_model_index_count);
	}
	*/
	

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
				mouse_input = sky_shader_program->get_input("mouse");
				resolution_input = sky_shader_program->get_input("resolution");
				time_input = sky_shader_program->get_input("time");
				exposure_input = sky_shader_program->get_input("camera.exposure");

				observer_altitude_input = sky_shader_program->get_input("observer_altitude");
				sun_direction_input = sky_shader_program->get_input("sun_direction");
				sun_color_input = sky_shader_program->get_input("sun_color");
				sun_angular_radius_input = sky_shader_program->get_input("sun_angular_radius");
				scale_height_rm_input = sky_shader_program->get_input("scale_height_rm");
				rayleigh_scattering_input = sky_shader_program->get_input("rayleigh_scattering");
				mie_scattering_input = sky_shader_program->get_input("mie_scattering");
				mie_anisotropy_input = sky_shader_program->get_input("mie_anisotropy");
				atmosphere_radii_input = sky_shader_program->get_input("atmosphere_radii");
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

void sky_pass::set_stars_model(const model* model)
{
	stars_model = model;
	
	if (stars_model)
	{
		stars_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			star_material = group->get_material();
			stars_model_drawing_mode = group->get_drawing_mode();
			stars_model_start_index = group->get_start_index();
			stars_model_index_count = group->get_index_count();
		}
		
		if (star_material)
		{
			star_shader_program = star_material->get_shader_program();
			
			if (star_shader_program)
			{
				star_model_view_input = star_shader_program->get_input("model_view");
				star_projection_input = star_shader_program->get_input("projection");
				star_distance_input = star_shader_program->get_input("star_distance");
				star_exposure_input = star_shader_program->get_input("camera.exposure");
			}
		}
	}
	else
	{
		stars_model = nullptr;
	}
}

void sky_pass::set_clouds_model(const model* model)
{
	clouds_model = model;
	
	if (clouds_model)
	{
		clouds_model_vao = model->get_vertex_array();

		const std::vector<model_group*>& groups = *model->get_groups();
		for (model_group* group: groups)
		{
			cloud_material = group->get_material();
			clouds_model_drawing_mode = group->get_drawing_mode();
			clouds_model_start_index = group->get_start_index();
			clouds_model_index_count = group->get_index_count();
		}
		
		if (cloud_material)
		{
			cloud_shader_program = cloud_material->get_shader_program();
			
			if (cloud_shader_program)
			{
				cloud_model_view_projection_input = cloud_shader_program->get_input("model_view_projection");
				cloud_sun_direction_input = cloud_shader_program->get_input("sun_direction");
				cloud_sun_color_input = cloud_shader_program->get_input("sun_color");
				cloud_camera_position_input = cloud_shader_program->get_input("camera.position");
				cloud_camera_exposure_input = cloud_shader_program->get_input("camera.exposure");
			}
		}
	}
	else
	{
		clouds_model = nullptr;
	}
}

void sky_pass::update_tweens()
{
	observer_altitude_tween.update();
	sun_position_tween.update();
	sun_color_outer_tween.update();
	sun_color_inner_tween.update();
	topocentric_frame_translation.update();
	topocentric_frame_rotation.update();
}

void sky_pass::set_topocentric_frame(const physics::frame<float>& frame)
{
	topocentric_frame_translation[1] = frame.translation;
	topocentric_frame_rotation[1] = frame.rotation;
}

void sky_pass::set_sun_position(const float3& position)
{
	sun_position_tween[1] = position;
}

void sky_pass::set_sun_color(const float3& color_outer, const float3& color_inner)
{
	sun_color_outer_tween[1] = color_outer;
	sun_color_inner_tween[1] = color_inner;
}

void sky_pass::set_sun_angular_radius(float radius)
{
	sun_angular_radius = radius;
}

void sky_pass::set_observer_altitude(float altitude)
{
	observer_altitude_tween[1] = altitude;
}

void sky_pass::set_scale_heights(float rayleigh, float mie)
{
	scale_height_rm = {rayleigh, mie};
}

void sky_pass::set_scattering_coefficients(const float3& r, const float3& m)
{
	rayleigh_scattering = r;
	mie_scattering = m;
}

void sky_pass::set_mie_anisotropy(float g)
{
	mie_anisotropy = {g, g * g};
}

void sky_pass::set_atmosphere_radii(float inner, float outer)
{
	atmosphere_radii.x = inner;
	atmosphere_radii.y = outer;
	atmosphere_radii.z = outer * outer;
}

void sky_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}

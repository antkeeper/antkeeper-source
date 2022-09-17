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

#include "render/passes/sky-pass.hpp"
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
#include "render/vertex-attribute.hpp"
#include "render/context.hpp"
#include "render/model.hpp"
#include "render/material.hpp"
#include "scene/camera.hpp"
#include "utility/fundamental-types.hpp"
#include "color/color.hpp"
#include "math/interpolation.hpp"
#include "geom/cartesian.hpp"
#include "geom/spherical.hpp"
#include "physics/orbit/orbit.hpp"
#include "physics/light/photometry.hpp"
#include <cmath>
#include <stdexcept>
#include <glad/glad.h>

namespace render {

sky_pass::sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
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
	sun_luminance_tween(float3{0.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	sun_illuminance_tween(float3{0.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	icrf_to_eus_translation({0, 0, 0}, math::lerp<float3, float>),
	icrf_to_eus_rotation(math::quaternion<float>::identity, math::nlerp<float>),
	moon_position_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_rotation_tween(math::quaternion<float>::identity, math::nlerp<float>),
	moon_angular_radius_tween(0.0f, math::lerp<float, float>),
	moon_sunlight_direction_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_sunlight_illuminance_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_planetlight_direction_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_planetlight_illuminance_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	magnification(1.0f)
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
	float4x4 model = math::scale(math::matrix4<float>::identity, model_scale);
	float4x4 view = math::resize<4, 4>(math::resize<3, 3>(camera.get_view_tween().interpolate(ctx.alpha)));
	float4x4 model_view = view * model;
	float4x4 projection = camera.get_projection_tween().interpolate(ctx.alpha);
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection = projection * model_view;
	
	// Interpolate observer altitude
	float observer_altitude = observer_altitude_tween.interpolate(ctx.alpha);
	
	// Construct tweened ICRF to EUS transformation
	math::transformation::se3<float> icrf_to_eus =
	{
		icrf_to_eus_translation.interpolate(ctx.alpha),
		icrf_to_eus_rotation.interpolate(ctx.alpha)
	};
	
	// Get EUS direction to sun
	float3 sun_position = sun_position_tween.interpolate(ctx.alpha);
	float3 sun_direction = math::normalize(sun_position);
	
	// Interpolate and expose sun luminance and illuminance
	float3 sun_luminance = sun_luminance_tween.interpolate(ctx.alpha) * ctx.exposure;
	float3 sun_illuminance = sun_illuminance_tween.interpolate(ctx.alpha) * ctx.exposure;
	
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
			exposure_input->upload(ctx.exposure);
		
		if (observer_altitude_input)
			observer_altitude_input->upload(observer_altitude);
		if (sun_direction_input)
			sun_direction_input->upload(sun_direction);
		if (sun_angular_radius_input)
			sun_angular_radius_input->upload(sun_angular_radius * magnification);
		
		if (sun_luminance_input)
			sun_luminance_input->upload(sun_luminance);
		if (sun_illuminance_input)
			sun_illuminance_input->upload(sun_illuminance);
		
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
		if (cloud_sun_illuminance_input)
			cloud_sun_illuminance_input->upload(sun_illuminance);
		if (cloud_camera_position_input)
			cloud_camera_position_input->upload(ctx.camera_transform.translation);
		if (cloud_camera_exposure_input)
			cloud_camera_exposure_input->upload(ctx.exposure);
		
		cloud_material->upload(ctx.alpha);

		rasterizer->draw_arrays(*clouds_model_vao, clouds_model_drawing_mode, clouds_model_start_index, clouds_model_index_count);
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_ONE, GL_ONE);
	
	// Draw stars
	if (stars_model)
	{
		float star_distance = (clip_near + clip_far) * 0.5f;
		
		model = math::resize<4, 4>(math::matrix_cast<float>(icrf_to_eus.r));
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
			star_exposure_input->upload(ctx.exposure);
		
		star_material->upload(ctx.alpha);
		
		rasterizer->draw_arrays(*stars_model_vao, stars_model_drawing_mode, stars_model_start_index, stars_model_index_count);
	}
	
	// Draw moon model
	float3 moon_position = moon_position_tween.interpolate(ctx.alpha);
	float moon_angular_radius = moon_angular_radius_tween.interpolate(ctx.alpha) * magnification;
	//if (moon_position.y >= -moon_angular_radius)
	{
		float moon_distance = (clip_near + clip_far) * 0.5f;		
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = math::normalize(moon_position) * moon_distance;
		moon_transform.rotation = moon_rotation_tween.interpolate(ctx.alpha);
		moon_transform.scale = {moon_radius, moon_radius, moon_radius};
		
		model = math::matrix_cast(moon_transform);		
		float3x3 normal_model = math::transpose(math::inverse(math::resize<3, 3>(model)));
		
		rasterizer->use_program(*moon_shader_program);
		if (moon_model_input)
			moon_model_input->upload(model);
		if (moon_view_projection_input)
			moon_view_projection_input->upload(view_projection);
		if (moon_normal_model_input)
			moon_normal_model_input->upload(normal_model);
		if (moon_camera_position_input)
			moon_camera_position_input->upload(ctx.camera_transform.translation);
		if (moon_sunlight_direction_input)
			moon_sunlight_direction_input->upload(math::normalize(moon_sunlight_direction_tween.interpolate(ctx.alpha)));
		if (moon_sunlight_illuminance_input)
			moon_sunlight_illuminance_input->upload(moon_sunlight_illuminance_tween.interpolate(ctx.alpha) * ctx.exposure);
		if (moon_planetlight_direction_input)
			moon_planetlight_direction_input->upload(math::normalize(moon_planetlight_direction_tween.interpolate(ctx.alpha)));
		if (moon_planetlight_illuminance_input)
			moon_planetlight_illuminance_input->upload(moon_planetlight_illuminance_tween.interpolate(ctx.alpha) * ctx.exposure);
		
		moon_material->upload(ctx.alpha);
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
				mouse_input = sky_shader_program->get_input("mouse");
				resolution_input = sky_shader_program->get_input("resolution");
				time_input = sky_shader_program->get_input("time");
				exposure_input = sky_shader_program->get_input("camera.exposure");

				observer_altitude_input = sky_shader_program->get_input("observer_altitude");
				sun_direction_input = sky_shader_program->get_input("sun_direction");
				sun_luminance_input = sky_shader_program->get_input("sun_luminance");
				sun_illuminance_input = sky_shader_program->get_input("sun_illuminance");
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
				moon_model_input = moon_shader_program->get_input("model");
				moon_view_projection_input = moon_shader_program->get_input("view_projection");
				moon_normal_model_input = moon_shader_program->get_input("normal_model");
				moon_camera_position_input = moon_shader_program->get_input("camera_position");
				moon_sunlight_direction_input = moon_shader_program->get_input("sunlight_direction");
				moon_sunlight_illuminance_input = moon_shader_program->get_input("sunlight_illuminance");
				moon_planetlight_direction_input = moon_shader_program->get_input("planetlight_direction");
				moon_planetlight_illuminance_input = moon_shader_program->get_input("planetlight_illuminance");
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
				cloud_sun_illuminance_input = cloud_shader_program->get_input("sun_illuminance");
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
	sun_luminance_tween.update();
	sun_illuminance_tween.update();
	icrf_to_eus_translation.update();
	icrf_to_eus_rotation.update();

	moon_position_tween.update();
	moon_rotation_tween.update();
	moon_angular_radius_tween.update();
	moon_sunlight_direction_tween.update();
	moon_sunlight_illuminance_tween.update();
	moon_planetlight_direction_tween.update();
	moon_planetlight_illuminance_tween.update();
}

void sky_pass::set_magnification(float magnification)
{
	this->magnification = magnification;
}

void sky_pass::set_icrf_to_eus(const math::transformation::se3<float>& transformation)
{
	icrf_to_eus_translation[1] = transformation.t;
	icrf_to_eus_rotation[1] = transformation.r;
}

void sky_pass::set_sun_position(const float3& position)
{
	sun_position_tween[1] = position;
}

void sky_pass::set_sun_illuminance(const float3& illuminance)
{
	sun_illuminance_tween[1] = illuminance;
}

void sky_pass::set_sun_luminance(const float3& luminance)
{
	sun_luminance_tween[1] = luminance;
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

void sky_pass::set_moon_position(const float3& position)
{
	moon_position_tween[1] = position;
}

void sky_pass::set_moon_rotation(const math::quaternion<float>& rotation)
{
	moon_rotation_tween[1] = rotation;
}

void sky_pass::set_moon_angular_radius(float angular_radius)
{
	moon_angular_radius_tween[1] = angular_radius;
}

void sky_pass::set_moon_sunlight_direction(const float3& direction)
{
	moon_sunlight_direction_tween[1] = direction;
}

void sky_pass::set_moon_sunlight_illuminance(const float3& illuminance)
{
	moon_sunlight_illuminance_tween[1] = illuminance;
}

void sky_pass::set_moon_planetlight_direction(const float3& direction)
{
	moon_planetlight_direction_tween[1] = direction;
}

void sky_pass::set_moon_planetlight_illuminance(const float3& illuminance)
{
	moon_planetlight_illuminance_tween[1] = illuminance;
}

void sky_pass::handle_event(const mouse_moved_event& event)
{
	mouse_position = {static_cast<float>(event.x), static_cast<float>(event.y)};
}

} // namespace render

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
	observer_position_tween({0, 0, 0}, math::lerp<float3, float>),
	sun_position_tween(float3{1.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	sun_luminance_tween(float3{0.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	sun_illuminance_tween(float3{0.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	icrf_to_eus_translation({0, 0, 0}, math::lerp<float3, float>),
	icrf_to_eus_rotation(math::quaternion<float>::identity(), math::nlerp<float>),
	moon_position_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_rotation_tween(math::quaternion<float>::identity(), math::nlerp<float>),
	moon_angular_radius_tween(0.0f, math::lerp<float, float>),
	moon_sunlight_direction_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_sunlight_illuminance_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_planetlight_direction_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_planetlight_illuminance_tween(float3{0, 0, 0}, math::lerp<float3, float>),
	moon_illuminance_tween(float3{0.0f, 0.0f, 0.0f}, math::lerp<float3, float>),
	render_transmittance_lut(false),
	magnification(1.0f)
{
	// Build quad VBO and VAO
	const float quad_vertex_data[] =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};
	std::size_t quad_vertex_size = 3;
	std::size_t quad_vertex_stride = sizeof(float) * quad_vertex_size;
	std::size_t quad_vertex_count = 6;
	quad_vbo = new gl::vertex_buffer(sizeof(float) * quad_vertex_size * quad_vertex_count, quad_vertex_data);
	quad_vao = new gl::vertex_array();
	gl::vertex_attribute quad_position_attribute;
	quad_position_attribute.buffer = quad_vbo;
	quad_position_attribute.offset = 0;
	quad_position_attribute.stride = quad_vertex_stride;
	quad_position_attribute.type = gl::vertex_attribute_type::float_32;
	quad_position_attribute.components = 3;
	quad_vao->bind(render::vertex_attribute::position, quad_position_attribute);
	
	// Create transmittance LUT texture and framebuffer (32F color, no depth)
	transmittance_lut_texture = new gl::texture_2d(256, 64, gl::pixel_type::float_32, gl::pixel_format::rgb);
	transmittance_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	transmittance_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	transmittance_lut_texture->set_max_anisotropy(0.0f);
	transmittance_lut_framebuffer = new gl::framebuffer({transmittance_lut_texture->get_width(), transmittance_lut_texture->get_height()});
	transmittance_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, transmittance_lut_texture);
	transmittance_lut_resolution = {static_cast<float>(transmittance_lut_texture->get_width()), static_cast<float>(transmittance_lut_texture->get_height())};
	
	// Load transmittance LUT shader template
	transmittance_shader_template = resource_manager->load<render::shader_template>("transmittance-lut.glsl");
	
	// Build transmittance LUT shader program
	transmittance_shader_program = transmittance_shader_template->build();
	transmittance_atmosphere_radii_input = transmittance_shader_program->get_input("atmosphere_radii");
	transmittance_rayleigh_parameters_input = transmittance_shader_program->get_input("rayleigh_parameters");
	transmittance_mie_parameters_input = transmittance_shader_program->get_input("mie_parameters");
	transmittance_ozone_distribution_input = transmittance_shader_program->get_input("ozone_distribution");
	transmittance_ozone_absorption_input = transmittance_shader_program->get_input("ozone_absorption");
	transmittance_resolution_input = transmittance_shader_program->get_input("resolution");
	
	// Create sky LUT texture and framebuffer (32F color, no depth)
	int sky_lut_width = 200;
	int sky_lut_height = 100;
	sky_lut_resolution = {static_cast<float>(sky_lut_width), static_cast<float>(sky_lut_height)};
	sky_lut_texture = new gl::texture_2d(sky_lut_width, sky_lut_height, gl::pixel_type::float_32, gl::pixel_format::rgb);
	sky_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	sky_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	sky_lut_texture->set_max_anisotropy(0.0f);
	sky_lut_framebuffer = new gl::framebuffer(sky_lut_width, sky_lut_height);
	sky_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, sky_lut_texture);
	
	// Load sky LUT shader template
	sky_lut_shader_template = resource_manager->load<render::shader_template>("sky-illuminance-lut.glsl");
	
	// Build sky LUT shader program
	sky_lut_shader_program = sky_lut_shader_template->build();
	sky_lut_light_direction_input = sky_lut_shader_program->get_input("light_direction");
	sky_lut_light_illuminance_input = sky_lut_shader_program->get_input("light_illuminance");
	sky_lut_atmosphere_radii_input = sky_lut_shader_program->get_input("atmosphere_radii");
	sky_lut_observer_position_input = sky_lut_shader_program->get_input("observer_position");
	sky_lut_rayleigh_parameters_input = sky_lut_shader_program->get_input("rayleigh_parameters");
	sky_lut_mie_parameters_input = sky_lut_shader_program->get_input("mie_parameters");
	sky_lut_ozone_distribution_input = sky_lut_shader_program->get_input("ozone_distribution");
	sky_lut_ozone_absorption_input = sky_lut_shader_program->get_input("ozone_absorption");
	sky_lut_airglow_illuminance_input = sky_lut_shader_program->get_input("airglow_illuminance");
	sky_lut_resolution_input = sky_lut_shader_program->get_input("resolution");
	sky_lut_transmittance_lut_input = sky_lut_shader_program->get_input("transmittance_lut");
	sky_lut_transmittance_lut_resolution_input = sky_lut_shader_program->get_input("transmittance_lut_resolution");
}

sky_pass::~sky_pass()
{
	delete sky_lut_framebuffer;
	delete sky_lut_texture;
	delete transmittance_lut_framebuffer;
	delete transmittance_lut_texture;
	delete quad_vao;
	delete quad_vbo;
	
	delete transmittance_shader_program;
	delete sky_lut_shader_program;
	
	/// @TODO
	// resource_maanger->unload("transmittance-lut.glsl");
	// resource_maanger->unload("sky-illuminance-lut.glsl");
}

void sky_pass::render(const render::context& ctx, render::queue& queue) const
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Render transmittance LUT if transmittance parameters have been altered.
	if (render_transmittance_lut)
	{
		// Render transmittance LUT
		rasterizer->set_viewport(0, 0, transmittance_lut_texture->get_width(), transmittance_lut_texture->get_height());
		rasterizer->use_framebuffer(*transmittance_lut_framebuffer);
		rasterizer->use_program(*transmittance_shader_program);
		transmittance_atmosphere_radii_input->upload(atmosphere_radii);
		transmittance_rayleigh_parameters_input->upload(rayleigh_parameters);
		transmittance_mie_parameters_input->upload(mie_parameters);
		transmittance_ozone_distribution_input->upload(ozone_distribution);
		transmittance_ozone_absorption_input->upload(ozone_absorption);
		if (transmittance_resolution_input)
			transmittance_resolution_input->upload(transmittance_lut_resolution);
		rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
		
		// Don't render transmittance LUT next frame unless parameters have changed.
		render_transmittance_lut = false;
	}
	
	// Construct matrices
	const scene::camera& camera = *ctx.camera;
	float clip_near = camera.get_clip_near_tween().interpolate(ctx.alpha);
	float clip_far = camera.get_clip_far_tween().interpolate(ctx.alpha);
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (clip_near + clip_far) * 0.5f;
	float4x4 model = math::scale(math::matrix4<float>::identity(), model_scale);
	float4x4 view = float4x4(float3x3(ctx.view));
	float4x4 model_view = view * model;
	const float4x4& projection = ctx.projection;
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection = projection * model_view;
	
	// Interpolate observer position
	float3 observer_position = observer_position_tween.interpolate(ctx.alpha);
	
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
	float3 sun_illuminance = sun_illuminance_tween.interpolate(ctx.alpha) * ctx.exposure;
	float3 sun_luminance = sun_luminance_tween.interpolate(ctx.alpha) * ctx.exposure;
	
	float3 moon_position = moon_position_tween.interpolate(ctx.alpha);
	float3 moon_direction = math::normalize(moon_position);
	float3 moon_illuminance = moon_illuminance_tween.interpolate(ctx.alpha) * ctx.exposure;
	float moon_angular_radius = moon_angular_radius_tween.interpolate(ctx.alpha) * magnification;
	
	float sun_y = color::aces::ap1<float>.luminance(sun_transmitted_illuminance);
	float moon_y = color::aces::ap1<float>.luminance(moon_transmitted_illuminance);
	float3 dominant_light_direction = (sun_y > moon_y) ? sun_direction : moon_direction;
	float3 dominant_light_illuminance = (sun_y > moon_y) ? sun_illuminance : moon_illuminance;
	
	if (moon_y > sun_y)
		sun_luminance *= 0.0f;
	
	// Render sky illuminance LUT
	auto sky_lut_viewport = sky_lut_framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(sky_lut_viewport), std::get<1>(sky_lut_viewport));
	rasterizer->use_framebuffer(*sky_lut_framebuffer);
	rasterizer->use_program(*sky_lut_shader_program);
	sky_lut_light_direction_input->upload(dominant_light_direction);
	sky_lut_light_illuminance_input->upload(dominant_light_illuminance);
	sky_lut_atmosphere_radii_input->upload(atmosphere_radii);
	sky_lut_observer_position_input->upload(observer_position);
	sky_lut_rayleigh_parameters_input->upload(rayleigh_parameters);
	sky_lut_mie_parameters_input->upload(mie_parameters);
	sky_lut_ozone_distribution_input->upload(ozone_distribution);
	sky_lut_ozone_absorption_input->upload(ozone_absorption);
	sky_lut_airglow_illuminance_input->upload(airglow_illuminance * ctx.exposure);
	if (sky_lut_resolution_input)
		sky_lut_resolution_input->upload(sky_lut_resolution);
	sky_lut_transmittance_lut_input->upload(transmittance_lut_texture);
	if (sky_lut_transmittance_lut_resolution_input)
		sky_lut_transmittance_lut_resolution_input->upload(transmittance_lut_resolution);
	rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
	
	
	
	rasterizer->use_framebuffer(*framebuffer);
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
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
		if (light_direction_input)
			light_direction_input->upload(dominant_light_direction);
		if (sun_luminance_input)
			sun_luminance_input->upload(sun_luminance);
		if (sun_angular_radius_input)
			sun_angular_radius_input->upload(sun_angular_radius * magnification);
		if (atmosphere_radii_input)
			atmosphere_radii_input->upload(atmosphere_radii);
		if (observer_position_input)
			observer_position_input->upload(observer_position);
		if (sky_illuminance_lut_input)
			sky_illuminance_lut_input->upload(sky_lut_texture);
		if (sky_illuminance_lut_resolution_input)
			sky_illuminance_lut_resolution_input->upload(sky_lut_resolution);
		
		sky_material->upload(ctx.alpha);

		rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_ONE, GL_ONE);
	
	// Draw stars
	if (stars_model)
	{
		float star_distance = (clip_near + clip_far) * 0.5f;
		
		model = float4x4(float3x3(icrf_to_eus.r));
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
	//if (moon_position.y() >= -moon_angular_radius)
	{
		float moon_distance = (clip_near + clip_far) * 0.5f;		
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = math::normalize(moon_position) * moon_distance;
		moon_transform.rotation = moon_rotation_tween.interpolate(ctx.alpha);
		moon_transform.scale = {moon_radius, moon_radius, moon_radius};
		
		model = math::matrix_cast(moon_transform);		
		float3x3 normal_model = math::transpose(math::inverse(float3x3(model)));
		
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
				light_direction_input = sky_shader_program->get_input("light_direction");
				sun_luminance_input = sky_shader_program->get_input("sun_luminance");
				sun_angular_radius_input = sky_shader_program->get_input("sun_angular_radius");
				atmosphere_radii_input = sky_shader_program->get_input("atmosphere_radii");
				observer_position_input = sky_shader_program->get_input("observer_position");
				sky_illuminance_lut_input = sky_shader_program->get_input("sky_illuminance_lut");
				sky_illuminance_lut_resolution_input = sky_shader_program->get_input("sky_illuminance_lut_resolution");
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

void sky_pass::update_tweens()
{
	observer_position_tween.update();
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
	moon_illuminance_tween.update();
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

void sky_pass::set_sun_illuminance(const float3& illuminance, const float3& transmitted_illuminance)
{
	sun_illuminance_tween[1] = illuminance;
	sun_transmitted_illuminance = transmitted_illuminance;
}

void sky_pass::set_sun_luminance(const float3& luminance)
{
	sun_luminance_tween[1] = luminance;
}

void sky_pass::set_sun_angular_radius(float radius)
{
	sun_angular_radius = radius;
}

void sky_pass::set_planet_radius(float radius)
{
	atmosphere_radii.x() = radius;
	atmosphere_radii.y() = atmosphere_radii.x() + atmosphere_upper_limit;
	atmosphere_radii.z() = atmosphere_radii.y() * atmosphere_radii.y();
	observer_position_tween[1] = {0.0f, atmosphere_radii.x() + observer_elevation, 0.0f};
	
	// Trigger transmittance LUT render
	render_transmittance_lut = true;
}

void sky_pass::set_atmosphere_upper_limit(float limit)
{
	atmosphere_upper_limit = limit;
	atmosphere_radii.y() = atmosphere_radii.x() + atmosphere_upper_limit;
	atmosphere_radii.z() = atmosphere_radii.y() * atmosphere_radii.y();
	
	// Trigger transmittance LUT render
	render_transmittance_lut = true;
}

void sky_pass::set_observer_elevation(float elevation)
{
	observer_elevation = elevation;
	observer_position_tween[1] = {0.0f, atmosphere_radii.x() + observer_elevation, 0.0f};
}

void sky_pass::set_rayleigh_parameters(float scale_height, const float3& scattering)
{
	rayleigh_parameters =
	{
		-1.0f / scale_height,
		scattering.x(),
		scattering.y(),
		scattering.z()
	};
	
	// Trigger transmittance LUT render
	render_transmittance_lut = true;
}

void sky_pass::set_mie_parameters(float scale_height, float scattering, float extinction, float anisotropy)
{
	mie_parameters =
	{
		-1.0f / scale_height,
		scattering,
		extinction,
		anisotropy
	};
	
	// Trigger transmittance LUT render
	render_transmittance_lut = true;
}

void sky_pass::set_ozone_parameters(float lower_limit, float upper_limit, float mode, const float3& absorption)
{
	ozone_distribution =
	{
		1.0f / (lower_limit - mode),
		1.0f / (upper_limit - mode),
		mode
	};
	ozone_absorption = absorption;
	
	// Trigger transmittance LUT render
	render_transmittance_lut = true;
}

void sky_pass::set_airglow_illuminance(const float3& illuminance)
{
	airglow_illuminance = illuminance;
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

void sky_pass::set_moon_illuminance(const float3& illuminance, const float3& transmitted_illuminance)
{
	moon_illuminance_tween[1] = illuminance;
	moon_transmitted_illuminance = transmitted_illuminance;
}

void sky_pass::set_transmittance_lut_resolution(std::uint16_t width, std::uint16_t height)
{
	transmittance_lut_texture->resize(width, height, nullptr);
	transmittance_lut_framebuffer->resize({transmittance_lut_texture->get_width(), transmittance_lut_texture->get_height()});
	transmittance_lut_resolution = {static_cast<float>(width), static_cast<float>(height)};
	
	// Trigger transmittance LUT render
	render_transmittance_lut = true;
}

} // namespace render

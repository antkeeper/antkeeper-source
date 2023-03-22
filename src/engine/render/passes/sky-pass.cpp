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

#include <engine/render/passes/sky-pass.hpp>
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
#include <engine/utility/fundamental-types.hpp>
#include <engine/color/color.hpp>
#include <engine/math/interpolation.hpp>
#include <engine/geom/cartesian.hpp>
#include <engine/geom/spherical.hpp>
#include <engine/physics/orbit/orbit.hpp>
#include <engine/physics/light/photometry.hpp>
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
	sky_lut_shader_program(nullptr),
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
	const float2 vertex_positions[] =
	{
		{-1.0f,  1.0f},
		{-1.0f, -1.0f},
		{ 1.0f,  1.0f},
		{ 1.0f,  1.0f},
		{-1.0f, -1.0f},
		{ 1.0f, -1.0f}
	};
	
	const auto vertex_data = std::as_bytes(std::span{vertex_positions});
	std::size_t vertex_size = 2;
	std::size_t vertex_stride = sizeof(float) * vertex_size;
	
	quad_vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data.size(), vertex_data);
	quad_vao = std::make_unique<gl::vertex_array>();
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = quad_vbo.get();
	position_attribute.offset = 0;
	position_attribute.stride = vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 2;
	
	// Bind vertex attributes to VAO
	quad_vao->bind(render::vertex_attribute::position, position_attribute);
	
	// Create transmittance LUT texture and framebuffer (32F color, no depth)
	transmittance_lut_texture = std::make_unique<gl::texture_2d>(256, 64, gl::pixel_type::float_32, gl::pixel_format::rgb);
	transmittance_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	transmittance_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	transmittance_lut_texture->set_max_anisotropy(0.0f);
	transmittance_lut_framebuffer = std::make_unique<gl::framebuffer>(transmittance_lut_texture->get_width(), transmittance_lut_texture->get_height());
	transmittance_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, transmittance_lut_texture.get());
	transmittance_lut_resolution = {static_cast<float>(transmittance_lut_texture->get_width()), static_cast<float>(transmittance_lut_texture->get_height())};
	
	// Load transmittance LUT shader template
	transmittance_lut_shader_template = resource_manager->load<gl::shader_template>("transmittance-lut.glsl");
	
	// Build transmittance LUT shader program
	transmittance_lut_shader_program = transmittance_lut_shader_template->build
	(
		{
			{"TRANSMITTANCE_LUT_SAMPLES", std::to_string(40)}
		}
	);
	if (!transmittance_lut_shader_program->linked())
	{
		debug::log::error("Failed to build transmittance LUT shader program: {}", transmittance_lut_shader_program->info());
		debug::log::warning("{}", transmittance_lut_shader_template->configure(gl::shader_stage::vertex));
	}
	
	// Build transmittance LUT command buffer
	rebuild_transmittance_lut_command_buffer();
	
	// Create sky LUT texture and framebuffer (32F color, no depth)
	int sky_lut_width = 200;
	int sky_lut_height = 100;
	sky_lut_resolution = {static_cast<float>(sky_lut_width), static_cast<float>(sky_lut_height)};
	sky_lut_texture = std::make_unique<gl::texture_2d>(sky_lut_width, sky_lut_height, gl::pixel_type::float_32, gl::pixel_format::rgb);
	sky_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
	sky_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
	sky_lut_texture->set_max_anisotropy(0.0f);
	sky_lut_framebuffer = std::make_unique<gl::framebuffer>(sky_lut_width, sky_lut_height);
	sky_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, sky_lut_texture.get());
	
	// Load sky LUT shader template
	sky_lut_shader_template = resource_manager->load<gl::shader_template>("sky-illuminance-lut.glsl");
	
	// Build sky LUT shader program
	sky_lut_shader_program = sky_lut_shader_template->build
	(
		{
			{"SKY_ILLUMINANCE_SAMPLES", std::to_string(30)}
		}
	);
	if (!sky_lut_shader_program->linked())
	{
		debug::log::error("Failed to by sky LUT shader program: {}", sky_lut_shader_program->info());
		debug::log::warning("{}", sky_lut_shader_template->configure(gl::shader_stage::vertex));
	}
	
	// Build sky LUT command buffer
	rebuild_sky_lut_command_buffer();
}

void sky_pass::render(render::context& ctx)
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
		for (const auto& command: transmittance_lut_command_buffer)
		{
			command();
		}
		
		// Don't render transmittance LUT next frame unless parameters have changed.
		render_transmittance_lut = false;
	}
	
	// Construct matrices
	const scene::camera& camera = *ctx.camera;
	float3 model_scale = float3{1.0f, 1.0f, 1.0f} * (camera.get_clip_near() + camera.get_clip_far()) * 0.5f;
	float4x4 model = math::scale(math::matrix4<float>::identity(), model_scale);
	float4x4 view = float4x4(float3x3(camera.get_view()));
	float4x4 model_view = view * model;
	const float4x4& projection = camera.get_projection();
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection = projection * model_view;
	
	// Interpolate observer position
	observer_position = observer_position_tween.interpolate(ctx.alpha);
	
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
	float3 sun_illuminance = sun_illuminance_tween.interpolate(ctx.alpha) * camera_exposure;
	float3 sun_luminance = sun_luminance_tween.interpolate(ctx.alpha) * camera_exposure;
	
	float3 moon_position = moon_position_tween.interpolate(ctx.alpha);
	float3 moon_direction = math::normalize(moon_position);
	float3 moon_illuminance = moon_illuminance_tween.interpolate(ctx.alpha) * camera_exposure;
	float moon_angular_radius = moon_angular_radius_tween.interpolate(ctx.alpha) * magnification;
	
	float sun_y = color::aces::ap1<float>.luminance(sun_transmitted_illuminance);
	float moon_y = color::aces::ap1<float>.luminance(moon_transmitted_illuminance);
	dominant_light_direction = (sun_y > moon_y) ? sun_direction : moon_direction;
	dominant_light_illuminance = (sun_y > moon_y) ? sun_illuminance : moon_illuminance;
	
	if (moon_y > sun_y)
	{
		sun_luminance *= 0.0f;
	}
	
	camera_exposure = camera.get_exposure_normalization();
	
	// Render sky illuminance LUT
	for (const auto& command: sky_lut_command_buffer)
	{
		command();
	}
	
	rasterizer->use_framebuffer(*framebuffer);
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	float2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
	// Draw atmosphere
	if (sky_model && sky_shader_program)
	{
		rasterizer->use_program(*sky_shader_program);

		// Upload shader parameters
		if (model_view_projection_var)
			model_view_projection_var->update(model_view_projection);
		if (mouse_var)
			mouse_var->update(mouse_position);
		if (resolution_var)
			resolution_var->update(resolution);
		if (light_direction_var)
			light_direction_var->update(dominant_light_direction);
		if (sun_luminance_var)
			sun_luminance_var->update(sun_luminance);
		if (sun_angular_radius_var)
			sun_angular_radius_var->update(sun_angular_radius * magnification);
		if (atmosphere_radii_var)
			atmosphere_radii_var->update(atmosphere_radii);
		if (observer_position_var)
			observer_position_var->update(observer_position);
		if (sky_illuminance_lut_var)
			sky_illuminance_lut_var->update(*sky_lut_texture);
		if (sky_illuminance_lut_resolution_var)
			sky_illuminance_lut_resolution_var->update(sky_lut_resolution);
		
		//sky_material->update(ctx.alpha);

		rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_ONE, GL_ONE);
	
	// Draw stars
	if (star_shader_program)
	{
		float star_distance = (camera.get_clip_near() + camera.get_clip_far()) * 0.5f;
		
		model = float4x4(float3x3(icrf_to_eus.r));
		model = math::scale(model, {star_distance, star_distance, star_distance});
		
		model_view = view * model;
		
		rasterizer->use_program(*star_shader_program);
		if (star_model_view_var)
			star_model_view_var->update(model_view);
		if (star_projection_var)
			star_projection_var->update(projection);
		if (star_distance_var)
			star_distance_var->update(star_distance);
		if (star_exposure_var)
			star_exposure_var->update(camera_exposure);
		
		//star_material->update(ctx.alpha);
		
		rasterizer->draw_arrays(*stars_model_vao, stars_model_drawing_mode, stars_model_start_index, stars_model_index_count);
	}
	
	// Draw moon model
	//if (moon_position.y() >= -moon_angular_radius)
	if (moon_shader_program)
	{
		float moon_distance = (camera.get_clip_near() + camera.get_clip_far()) * 0.5f;
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = math::normalize(moon_position) * moon_distance;
		moon_transform.rotation = moon_rotation_tween.interpolate(ctx.alpha);
		moon_transform.scale = {moon_radius, moon_radius, moon_radius};
		
		model = moon_transform.matrix();		
		float3x3 normal_model = math::transpose(math::inverse(float3x3(model)));
		
		rasterizer->use_program(*moon_shader_program);
		if (moon_model_var)
			moon_model_var->update(model);
		if (moon_view_projection_var)
			moon_view_projection_var->update(view_projection);
		if (moon_normal_model_var)
			moon_normal_model_var->update(normal_model);
		if (moon_camera_position_var)
			moon_camera_position_var->update(camera.get_translation());
		if (moon_sunlight_direction_var)
			moon_sunlight_direction_var->update(math::normalize(moon_sunlight_direction_tween.interpolate(ctx.alpha)));
		if (moon_sunlight_illuminance_var)
			moon_sunlight_illuminance_var->update(moon_sunlight_illuminance_tween.interpolate(ctx.alpha) * camera_exposure);
		if (moon_planetlight_direction_var)
			moon_planetlight_direction_var->update(math::normalize(moon_planetlight_direction_tween.interpolate(ctx.alpha)));
		if (moon_planetlight_illuminance_var)
			moon_planetlight_illuminance_var->update(moon_planetlight_illuminance_tween.interpolate(ctx.alpha) * camera_exposure);
		
		//moon_material->update(ctx.alpha);
		rasterizer->draw_arrays(*moon_model_vao, moon_model_drawing_mode, moon_model_start_index, moon_model_index_count);
	}
}

void sky_pass::set_sky_model(std::shared_ptr<render::model> model)
{
	sky_model = model;
	sky_shader_program = nullptr;
	
	if (sky_model)
	{
		sky_model_vao = model->get_vertex_array().get();
		
		for (const auto& group: model->get_groups())
		{
			sky_material = group.material.get();
			sky_model_drawing_mode = group.drawing_mode;
			sky_model_start_index = group.start_index;
			sky_model_index_count = group.index_count;
		}
		
		
		if (sky_material)
		{
			sky_shader_program = sky_material->get_shader_template()->build();
			
			if (sky_shader_program)
			{
				model_view_projection_var = sky_shader_program->variable("model_view_projection");
				mouse_var = sky_shader_program->variable("mouse");
				resolution_var = sky_shader_program->variable("resolution");
				light_direction_var = sky_shader_program->variable("light_direction");
				sun_luminance_var = sky_shader_program->variable("sun_luminance");
				sun_angular_radius_var = sky_shader_program->variable("sun_angular_radius");
				atmosphere_radii_var = sky_shader_program->variable("atmosphere_radii");
				observer_position_var = sky_shader_program->variable("observer_position");
				sky_illuminance_lut_var = sky_shader_program->variable("sky_illuminance_lut");
				sky_illuminance_lut_resolution_var = sky_shader_program->variable("sky_illuminance_lut_resolution");
			}
		}
	}
	else
	{
		sky_model_vao = nullptr;
	}
}

void sky_pass::set_moon_model(std::shared_ptr<render::model> model)
{
	moon_model = model;
	moon_shader_program = nullptr;
	
	if (moon_model)
	{
		moon_model_vao = model->get_vertex_array().get();
		
		for (const auto& group: model->get_groups())
		{
			moon_material = group.material.get();
			moon_model_drawing_mode = group.drawing_mode;
			moon_model_start_index = group.start_index;
			moon_model_index_count = group.index_count;
		}
		
		if (moon_material)
		{
			moon_shader_program = moon_material->get_shader_template()->build();	
			
			if (moon_shader_program)
			{
				moon_model_var = moon_shader_program->variable("model");
				moon_view_projection_var = moon_shader_program->variable("view_projection");
				moon_normal_model_var = moon_shader_program->variable("normal_model");
				moon_camera_position_var = moon_shader_program->variable("camera_position");
				moon_sunlight_direction_var = moon_shader_program->variable("sunlight_direction");
				moon_sunlight_illuminance_var = moon_shader_program->variable("sunlight_illuminance");
				moon_planetlight_direction_var = moon_shader_program->variable("planetlight_direction");
				moon_planetlight_illuminance_var = moon_shader_program->variable("planetlight_illuminance");
			}
		}
	}
	else
	{
		moon_model = nullptr;
	}
}

void sky_pass::set_stars_model(std::shared_ptr<render::model> model)
{
	stars_model = model;
	star_shader_program = nullptr;
	
	if (stars_model)
	{
		stars_model_vao = model->get_vertex_array().get();
		
		for (const auto& group: model->get_groups())
		{
			stars_model_drawing_mode = group.drawing_mode;
			stars_model_start_index = group.start_index;
			stars_model_index_count = group.index_count;
		}
		
		if (star_material)
		{
			star_shader_program = star_material->get_shader_template()->build();
			
			if (star_shader_program)
			{
				star_model_view_var = star_shader_program->variable("model_view");
				star_projection_var = star_shader_program->variable("projection");
				star_distance_var = star_shader_program->variable("star_distance");
				star_exposure_var = star_shader_program->variable("camera.exposure");
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

void sky_pass::rebuild_transmittance_lut_command_buffer()
{
	transmittance_lut_command_buffer.clear();
	
	if (!transmittance_lut_shader_program->linked() || !transmittance_lut_texture)
	{
		return;
	}
	
	// Bind framebuffer and shader program
	transmittance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->set_viewport(0, 0, transmittance_lut_texture->get_width(), transmittance_lut_texture->get_height());
			rasterizer->use_framebuffer(*transmittance_lut_framebuffer);
			rasterizer->use_program(*transmittance_lut_shader_program);
		}
	);
	
	// Update shader variables
	if (auto atmosphere_radii_var = transmittance_lut_shader_program->variable("atmosphere_radii"))
	{
		transmittance_lut_command_buffer.emplace_back([&, atmosphere_radii_var](){atmosphere_radii_var->update(atmosphere_radii);});
	}
	if (auto rayleigh_parameters_var = transmittance_lut_shader_program->variable("rayleigh_parameters"))
	{
		transmittance_lut_command_buffer.emplace_back([&, rayleigh_parameters_var](){rayleigh_parameters_var->update(rayleigh_parameters);});
	}
	if (auto mie_parameters_var = transmittance_lut_shader_program->variable("mie_parameters"))
	{
		transmittance_lut_command_buffer.emplace_back([&, mie_parameters_var](){mie_parameters_var->update(mie_parameters);});
	}
	if (auto ozone_distribution_var = transmittance_lut_shader_program->variable("ozone_distribution"))
	{
		transmittance_lut_command_buffer.emplace_back([&, ozone_distribution_var](){ozone_distribution_var->update(ozone_distribution);});
	}
	if (auto ozone_absorption_var = transmittance_lut_shader_program->variable("ozone_absorption"))
	{
		transmittance_lut_command_buffer.emplace_back([&, ozone_absorption_var](){ozone_absorption_var->update(ozone_absorption);});
	}
	if (auto resolution_var = transmittance_lut_shader_program->variable("resolution"))
	{
		transmittance_lut_command_buffer.emplace_back([&, resolution_var](){resolution_var->update(transmittance_lut_resolution);});
	}
	
	// Draw quad
	transmittance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
		}
	);
}

void sky_pass::rebuild_sky_lut_command_buffer()
{
	sky_lut_command_buffer.clear();
	
	if (!sky_lut_shader_program->linked() || !sky_lut_texture)
	{
		return;
	}
	
	// Bind framebuffer and shader program
	sky_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->set_viewport(0, 0, sky_lut_texture->get_width(), sky_lut_texture->get_height());
			rasterizer->use_framebuffer(*sky_lut_framebuffer);
			rasterizer->use_program(*sky_lut_shader_program);
		}
	);
	
	// Update shader variables
	if (auto light_direction_var = sky_lut_shader_program->variable("light_direction"))
	{
		sky_lut_command_buffer.emplace_back([&, light_direction_var](){light_direction_var->update(dominant_light_direction);});
	}
	if (auto light_illuminance_var = sky_lut_shader_program->variable("light_illuminance"))
	{
		sky_lut_command_buffer.emplace_back([&, light_illuminance_var](){light_illuminance_var->update(dominant_light_illuminance);});
	}
	if (auto atmosphere_radii_var = sky_lut_shader_program->variable("atmosphere_radii"))
	{
		sky_lut_command_buffer.emplace_back([&, atmosphere_radii_var](){atmosphere_radii_var->update(atmosphere_radii);});
	}
	if (auto observer_position_var = sky_lut_shader_program->variable("observer_position"))
	{
		sky_lut_command_buffer.emplace_back([&, observer_position_var](){observer_position_var->update(observer_position);});
	}
	if (auto rayleigh_parameters_var = sky_lut_shader_program->variable("rayleigh_parameters"))
	{
		sky_lut_command_buffer.emplace_back([&, rayleigh_parameters_var](){rayleigh_parameters_var->update(rayleigh_parameters);});
	}
	if (auto mie_parameters_var = sky_lut_shader_program->variable("mie_parameters"))
	{
		sky_lut_command_buffer.emplace_back([&, mie_parameters_var](){mie_parameters_var->update(mie_parameters);});
	}
	if (auto ozone_distribution_var = sky_lut_shader_program->variable("ozone_distribution"))
	{
		sky_lut_command_buffer.emplace_back([&, ozone_distribution_var](){ozone_distribution_var->update(ozone_distribution);});
	}
	if (auto ozone_absorption_var = sky_lut_shader_program->variable("ozone_absorption"))
	{
		sky_lut_command_buffer.emplace_back([&, ozone_absorption_var](){ozone_absorption_var->update(ozone_absorption);});
	}
	if (auto airglow_illuminance_var = sky_lut_shader_program->variable("airglow_illuminance"))
	{
		sky_lut_command_buffer.emplace_back([&, airglow_illuminance_var](){airglow_illuminance_var->update(airglow_illuminance * camera_exposure);});
	}
	if (auto resolution_var = sky_lut_shader_program->variable("resolution"))
	{
		sky_lut_command_buffer.emplace_back([&, resolution_var](){resolution_var->update(sky_lut_resolution);});
	}
	if (auto transmittance_lut_var = sky_lut_shader_program->variable("transmittance_lut"))
	{
		sky_lut_command_buffer.emplace_back([&, transmittance_lut_var](){transmittance_lut_var->update(*transmittance_lut_texture);});
	}
	if (auto transmittance_lut_resolution_var = sky_lut_shader_program->variable("transmittance_lut_resolution"))
	{
		sky_lut_command_buffer.emplace_back([&, transmittance_lut_resolution_var](){transmittance_lut_resolution_var->update(transmittance_lut_resolution);});
	}
	
	// Draw quad
	sky_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
		}
	);
}

} // namespace render

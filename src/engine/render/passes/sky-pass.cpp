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
#include <engine/math/vector.hpp>
#include <engine/color/color.hpp>
#include <engine/math/interpolation.hpp>
#include <engine/physics/orbit/orbit.hpp>
#include <engine/physics/light/photometry.hpp>
#include <bit>
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
	moon_model(nullptr),
	moon_model_vao(nullptr),
	moon_material(nullptr),
	moon_shader_program(nullptr),
	stars_model(nullptr),
	stars_model_vao(nullptr),
	star_material(nullptr),
	star_shader_program(nullptr),
	observer_position_tween({0, 0, 0}, math::lerp<math::fvec3, float>),
	sun_position_tween(math::fvec3{1.0f, 0.0f, 0.0f}, math::lerp<math::fvec3, float>),
	sun_luminance_tween(math::fvec3{0.0f, 0.0f, 0.0f}, math::lerp<math::fvec3, float>),
	sun_illuminance_tween(math::fvec3{0.0f, 0.0f, 0.0f}, math::lerp<math::fvec3, float>),
	icrf_to_eus_translation({0, 0, 0}, math::lerp<math::fvec3, float>),
	icrf_to_eus_rotation(math::fquat::identity(), math::nlerp<float>),
	moon_position_tween(math::fvec3{0, 0, 0}, math::lerp<math::fvec3, float>),
	moon_rotation_tween(math::fquat::identity(), math::nlerp<float>),
	moon_angular_radius_tween(0.0f, math::lerp<float, float>),
	moon_sunlight_direction_tween(math::fvec3{0, 0, 0}, math::lerp<math::fvec3, float>),
	moon_sunlight_illuminance_tween(math::fvec3{0, 0, 0}, math::lerp<math::fvec3, float>),
	moon_planetlight_direction_tween(math::fvec3{0, 0, 0}, math::lerp<math::fvec3, float>),
	moon_planetlight_illuminance_tween(math::fvec3{0, 0, 0}, math::lerp<math::fvec3, float>),
	moon_illuminance_tween(math::fvec3{0.0f, 0.0f, 0.0f}, math::lerp<math::fvec3, float>),
	magnification(1.0f)
{
	// Build quad VBO and VAO
	const math::fvec2 vertex_positions[] =
	{
		{-1.0f,  1.0f},
		{-1.0f, -1.0f},
		{ 1.0f,  1.0f},
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
	
	// Transmittance LUT
	{
		// Construct transmittance LUT texture
		m_transmittance_lut_texture = std::make_unique<gl::texture_2d>(m_transmittance_lut_resolution.x(), m_transmittance_lut_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::rgb);
		m_transmittance_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
		m_transmittance_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
		m_transmittance_lut_texture->set_max_anisotropy(0.0f);
		
		// Construct transmittance LUT framebuffer and attach texture
		m_transmittance_lut_framebuffer = std::make_unique<gl::framebuffer>(m_transmittance_lut_resolution.x(), m_transmittance_lut_resolution.y());
		m_transmittance_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, m_transmittance_lut_texture.get());
		
		// Load transmittance LUT shader template
		m_transmittance_lut_shader_template = resource_manager->load<gl::shader_template>("sky-transmittance-lut.glsl");
		
		// Build transmittance LUT shader program
		rebuild_transmittance_lut_shader_program();
		
		// Build transmittance LUT command buffer
		rebuild_transmittance_lut_command_buffer();
	}
	
	// Multiscattering LUT
	{
		// Construct multiscattering LUT texture
		m_multiscattering_lut_texture = std::make_unique<gl::texture_2d>(m_multiscattering_lut_resolution.x(), m_multiscattering_lut_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::rgb);
		m_multiscattering_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
		m_multiscattering_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
		m_multiscattering_lut_texture->set_max_anisotropy(0.0f);
		
		// Construct multiscattering LUT framebuffer and attach texture
		m_multiscattering_lut_framebuffer = std::make_unique<gl::framebuffer>(m_multiscattering_lut_resolution.x(), m_multiscattering_lut_resolution.y());
		m_multiscattering_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, m_multiscattering_lut_texture.get());
		
		// Load multiscattering LUT shader template
		m_multiscattering_lut_shader_template = resource_manager->load<gl::shader_template>("sky-multiscattering-lut.glsl");
		
		// Build multiscattering LUT shader program
		rebuild_multiscattering_lut_shader_program();
		
		// Build multiscattering LUT command buffer
		rebuild_multiscattering_lut_command_buffer();
	}
	
	// Luminance LUT
	{
		// Construct luminance LUT texture
		m_luminance_lut_texture = std::make_unique<gl::texture_2d>(m_luminance_lut_resolution.x(), m_luminance_lut_resolution.y(), gl::pixel_type::float_32, gl::pixel_format::rgb);
		m_luminance_lut_texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
		m_luminance_lut_texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
		m_luminance_lut_texture->set_max_anisotropy(0.0f);
		
		// Construct luminance LUT framebuffer and attach texture
		m_luminance_lut_framebuffer = std::make_unique<gl::framebuffer>(m_luminance_lut_resolution.x(), m_luminance_lut_resolution.y());
		m_luminance_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, m_luminance_lut_texture.get());
		
		// Load luminance LUT shader template
		m_luminance_lut_shader_template = resource_manager->load<gl::shader_template>("sky-luminance-lut.glsl");
		
		// Build luminance LUT shader program
		rebuild_luminance_lut_shader_program();
		
		// Build luminance LUT command buffer
		rebuild_luminance_lut_command_buffer();
	}
	
	// Load sky probe shader template
	m_sky_probe_shader_template = resource_manager->load<gl::shader_template>("sky-probe.glsl");
	
	// Build sky probe shader program
	m_sky_probe_shader_program = m_sky_probe_shader_template->build({});
	if (!m_sky_probe_shader_program->linked())
	{
		debug::log::error("Failed to build sky probe shader program: {}", m_sky_probe_shader_program->info());
		debug::log::warning("{}", m_sky_probe_shader_template->configure(gl::shader_stage::vertex));
	}
	
	// Load moon textures
	m_moon_albedo_map = resource_manager->load<gl::texture_2d>("moon-albedo.tex");
	m_moon_normal_map = resource_manager->load<gl::texture_2d>("moon-normal.tex");
}

void sky_pass::render(render::context& ctx)
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Render transmittance LUT (if parameters have changed)
	if (m_render_transmittance_lut)
	{
		for (const auto& command: m_transmittance_lut_command_buffer)
		{
			command();
		}
		
		m_render_transmittance_lut = false;
	}
	
	// Render multiscattering LUT (if parameters have changed)
	if (m_render_multiscattering_lut)
	{
		for (const auto& command: m_multiscattering_lut_command_buffer)
		{
			command();
		}
		
		m_render_multiscattering_lut = false;
	}
	
	// Construct matrices
	const scene::camera& camera = *ctx.camera;
	math::fvec3 model_scale = math::fvec3{1.0f, 1.0f, 1.0f} * (camera.get_clip_near() + camera.get_clip_far()) * 0.5f;
	math::fmat4 model = math::scale(math::fmat4::identity(), model_scale);
	math::fmat4 view = math::fmat4(math::fmat3(camera.get_view()));
	math::fmat4 model_view = view * model;
	const math::fmat4& projection = camera.get_projection();
	math::fmat4 view_projection = projection * view;
	math::fmat4 model_view_projection = projection * model_view;
	camera_exposure = camera.get_exposure_normalization();
	
	// Interpolate observer position
	observer_position = observer_position_tween.interpolate(ctx.alpha);
	
	// Construct tweened ICRF to EUS transformation
	math::se3<float> icrf_to_eus =
	{
		icrf_to_eus_translation.interpolate(ctx.alpha),
		icrf_to_eus_rotation.interpolate(ctx.alpha)
	};
	
	// Get EUS direction to sun
	math::fvec3 sun_position = sun_position_tween.interpolate(ctx.alpha);
	math::fvec3 sun_direction = math::normalize(sun_position);
	
	// Interpolate and expose sun luminance and illuminance
	math::fvec3 sun_illuminance = sun_illuminance_tween.interpolate(ctx.alpha) * camera_exposure;
	math::fvec3 sun_luminance = sun_luminance_tween.interpolate(ctx.alpha) * camera_exposure;
	
	math::fvec3 moon_position = moon_position_tween.interpolate(ctx.alpha);
	math::fvec3 moon_direction = math::normalize(moon_position);
	math::fvec3 moon_illuminance = moon_illuminance_tween.interpolate(ctx.alpha) * camera_exposure;
	float moon_angular_radius = moon_angular_radius_tween.interpolate(ctx.alpha) * magnification;
	
	float sun_y = color::aces::ap1<float>.luminance(sun_transmitted_illuminance);
	float moon_y = color::aces::ap1<float>.luminance(moon_transmitted_illuminance);
	
	// if (math::max(sun_illuminance) > math::max(moon_illuminance))
	// {
		dominant_light_direction = sun_direction;
		dominant_light_illuminance = sun_illuminance;
	// }
	// else
	// {
		// dominant_light_direction = moon_direction;
		// dominant_light_illuminance = moon_illuminance;
	// }
	
	
	// Render luminance LUT
	// if (m_render_luminance_lut)
	{
		for (const auto& command: m_luminance_lut_command_buffer)
		{
			command();
		}
	}
	
	// Render sky probe
	for (const auto& command: m_sky_probe_command_buffer)
	{
		command();
	}
	
	rasterizer->use_framebuffer(*framebuffer);
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	math::fvec2 resolution = {static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))};
	
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
		if (sky_transmittance_lut_var)
			sky_transmittance_lut_var->update(*m_transmittance_lut_texture);
		if (sky_transmittance_lut_resolution_var)
			sky_transmittance_lut_resolution_var->update(math::fvec2(m_transmittance_lut_resolution));
		if (sky_luminance_lut_var)
			sky_luminance_lut_var->update(*m_luminance_lut_texture);
		if (sky_luminance_lut_resolution_var)
			sky_luminance_lut_resolution_var->update(math::fvec2(m_luminance_lut_resolution));
		
		//sky_material->update(ctx.alpha);

		rasterizer->draw_arrays(*sky_model_vao, sky_model_drawing_mode, sky_model_start_index, sky_model_index_count);
	}
	
	glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendFunc(GL_ONE, GL_ONE);
	
	// Flag moon pixels in stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilMask(0xff);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
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
		math::fmat3 normal_model = math::transpose(math::inverse(math::fmat3(model)));
		
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
		if (moon_albedo_map_var && m_moon_albedo_map)
			moon_albedo_map_var->update(*m_moon_albedo_map);
		if (moon_normal_map_var && m_moon_normal_map)
			moon_normal_map_var->update(*m_moon_normal_map);
		if (moon_observer_position_var)
			moon_observer_position_var->update(observer_position);
		if (moon_sky_transmittance_lut_var)
			moon_sky_transmittance_lut_var->update(*m_transmittance_lut_texture);
		if (moon_atmosphere_radii_var)
			moon_atmosphere_radii_var->update(atmosphere_radii);
		
		//moon_material->update(ctx.alpha);
		rasterizer->draw_arrays(*moon_model_vao, moon_model_drawing_mode, moon_model_start_index, moon_model_index_count);
	}
	
	// Prevent stars from being drawn in front of the moon
	glStencilMask(0x00);
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	
	// Draw stars
	if (star_shader_program)
	{
		float star_distance = (camera.get_clip_near() + camera.get_clip_far()) * 0.5f;
		
		model = math::fmat4(math::fmat3(icrf_to_eus.r));
		model = math::scale(model, {star_distance, star_distance, star_distance});
		
		model_view_projection = view_projection * model;
		
		rasterizer->use_program(*star_shader_program);
		if (star_model_view_projection_var)
			star_model_view_projection_var->update(model_view_projection);
		if (star_exposure_var)
			star_exposure_var->update(camera_exposure);
		if (star_inv_resolution_var)
			star_inv_resolution_var->update(1.0f / resolution);
		
		//star_material->update(ctx.alpha);
		
		rasterizer->draw_arrays(*stars_model_vao, stars_model_drawing_mode, stars_model_start_index, stars_model_index_count);
	}
	
	glDisable(GL_STENCIL_TEST);
}

void sky_pass::set_transmittance_lut_sample_count(std::uint16_t count)
{
	if (m_transmittance_lut_sample_count != count)
	{
		m_transmittance_lut_sample_count = count;
		
		// Rebuild transmittance LUT shader program and command buffer
		rebuild_transmittance_lut_shader_program();
		rebuild_transmittance_lut_command_buffer();
		
		// Trigger rendering of transmittance LUT
		m_render_transmittance_lut = true;
	}
}

void sky_pass::set_transmittance_lut_resolution(const math::vec2<std::uint16_t>& resolution)
{
	if (m_transmittance_lut_resolution.x() != resolution.x() || m_transmittance_lut_resolution.y() != resolution.y())
	{
		m_transmittance_lut_resolution = resolution;
		m_transmittance_lut_texture->resize(resolution.x(), resolution.y(), nullptr);
		m_transmittance_lut_framebuffer->resize({resolution.x(), resolution.y()});
		
		// Trigger rendering of transmittance LUT
		m_render_transmittance_lut = true;
	}
}

void sky_pass::set_multiscattering_lut_direction_sample_count(std::uint16_t count)
{
	if (m_multiscattering_lut_direction_sample_count != count)
	{
		m_multiscattering_lut_direction_sample_count = count;
		
		// Rebuild multiscattering LUT shader program and command buffer
		rebuild_multiscattering_lut_shader_program();
		rebuild_multiscattering_lut_command_buffer();
		
		// Trigger rendering of multiscattering LUT
		m_render_multiscattering_lut = true;
	}
}

void sky_pass::set_multiscattering_lut_scatter_sample_count(std::uint16_t count)
{
	if (m_multiscattering_lut_scatter_sample_count != count)
	{
		m_multiscattering_lut_scatter_sample_count = count;
		
		// Rebuild multiscattering LUT shader program and command buffer
		rebuild_multiscattering_lut_shader_program();
		rebuild_multiscattering_lut_command_buffer();
		
		// Trigger rendering of multiscattering LUT
		m_render_multiscattering_lut = true;
	}
}

void sky_pass::set_multiscattering_lut_resolution(const math::vec2<std::uint16_t>& resolution)
{
	if (m_multiscattering_lut_resolution.x() != resolution.x() || m_multiscattering_lut_resolution.y() != resolution.y())
	{
		m_multiscattering_lut_resolution = resolution;
		m_multiscattering_lut_texture->resize(resolution.x(), resolution.y(), nullptr);
		m_multiscattering_lut_framebuffer->resize({resolution.x(), resolution.y()});
		
		// Trigger rendering of multiscattering LUT
		m_render_multiscattering_lut = true;
	}
}

void sky_pass::set_luminance_lut_sample_count(std::uint16_t count)
{
	if (m_luminance_lut_sample_count != count)
	{
		m_luminance_lut_sample_count = count;
		
		// Rebuild luminance LUT shader program and command buffer
		rebuild_luminance_lut_shader_program();
		rebuild_luminance_lut_command_buffer();
		
		// Trigger rendering of luminance LUT
		m_render_luminance_lut = true;
	}
}

void sky_pass::set_luminance_lut_resolution(const math::vec2<std::uint16_t>& resolution)
{
	if (m_luminance_lut_resolution.x() != resolution.x() || m_luminance_lut_resolution.y() != resolution.y())
	{
		m_luminance_lut_resolution = resolution;
		m_luminance_lut_texture->resize(resolution.x(), resolution.y(), nullptr);
		m_luminance_lut_framebuffer->resize({resolution.x(), resolution.y()});
		
		// Trigger rendering of luminance LUT
		m_render_luminance_lut = true;
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
			
			if (sky_shader_program->linked())
			{
				model_view_projection_var = sky_shader_program->variable("model_view_projection");
				mouse_var = sky_shader_program->variable("mouse");
				resolution_var = sky_shader_program->variable("resolution");
				light_direction_var = sky_shader_program->variable("light_direction");
				sun_luminance_var = sky_shader_program->variable("sun_luminance");
				sun_angular_radius_var = sky_shader_program->variable("sun_angular_radius");
				atmosphere_radii_var = sky_shader_program->variable("atmosphere_radii");
				observer_position_var = sky_shader_program->variable("observer_position");
				sky_transmittance_lut_var = sky_shader_program->variable("sky_transmittance_lut");
				sky_transmittance_lut_resolution_var = sky_shader_program->variable("sky_transmittance_lut_resolution");
				sky_luminance_lut_var = sky_shader_program->variable("sky_luminance_lut");
				sky_luminance_lut_resolution_var = sky_shader_program->variable("sky_luminance_lut_resolution");
			}
			else
			{
				debug::log::error("Failed to build sky shader program: {}", sky_shader_program->info());
				debug::log::warning("{}", sky_material->get_shader_template()->configure(gl::shader_stage::vertex));
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
			
			if (moon_shader_program->linked())
			{
				moon_model_var = moon_shader_program->variable("model");
				moon_view_projection_var = moon_shader_program->variable("view_projection");
				moon_normal_model_var = moon_shader_program->variable("normal_model");
				moon_camera_position_var = moon_shader_program->variable("camera_position");
				moon_sunlight_direction_var = moon_shader_program->variable("sunlight_direction");
				moon_sunlight_illuminance_var = moon_shader_program->variable("sunlight_illuminance");
				moon_planetlight_direction_var = moon_shader_program->variable("planetlight_direction");
				moon_planetlight_illuminance_var = moon_shader_program->variable("planetlight_illuminance");
				moon_albedo_map_var = moon_shader_program->variable("albedo_map");
				moon_normal_map_var = moon_shader_program->variable("normal_map");
				moon_observer_position_var = moon_shader_program->variable("observer_position");
				moon_sky_transmittance_lut_var = moon_shader_program->variable("sky_transmittance_lut");
				moon_atmosphere_radii_var = moon_shader_program->variable("atmosphere_radii");
			}
			else
			{
				debug::log::error("Failed to build moon shader program: {}", moon_shader_program->info());
				debug::log::warning("{}", moon_material->get_shader_template()->configure(gl::shader_stage::vertex));
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
			star_material = group.material.get();
		}
		
		if (star_material)
		{
			star_shader_program = star_material->get_shader_template()->build();
			
			if (star_shader_program->linked())
			{
				star_model_view_projection_var = star_shader_program->variable("model_view_projection");
				star_exposure_var = star_shader_program->variable("camera_exposure");
				star_inv_resolution_var = star_shader_program->variable("inv_resolution");
			}
			else
			{
				debug::log::error("Failed to build star shader program: {}", star_shader_program->info());
				debug::log::warning("{}", star_material->get_shader_template()->configure(gl::shader_stage::vertex));
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

void sky_pass::set_icrf_to_eus(const math::se3<float>& transformation)
{
	icrf_to_eus_translation[1] = transformation.t;
	icrf_to_eus_rotation[1] = transformation.r;
}

void sky_pass::set_sun_position(const math::fvec3& position)
{
	sun_position_tween[1] = position;
}

void sky_pass::set_sun_illuminance(const math::fvec3& illuminance, const math::fvec3& transmitted_illuminance)
{
	sun_illuminance_tween[1] = illuminance;
	sun_transmitted_illuminance = transmitted_illuminance;
}

void sky_pass::set_sun_luminance(const math::fvec3& luminance)
{
	sun_luminance_tween[1] = luminance;
}

void sky_pass::set_sun_angular_radius(float radius)
{
	sun_angular_radius = radius;
}

void sky_pass::set_planet_radius(float radius)
{
	atmosphere_radii[0] = radius;
	atmosphere_radii[1] = atmosphere_radii[0] + atmosphere_upper_limit;
	atmosphere_radii[2] = atmosphere_radii[0] * atmosphere_radii[0];
	atmosphere_radii[3] = atmosphere_radii[1] * atmosphere_radii[1];
	
	observer_position_tween[1] = {0.0f, atmosphere_radii.x() + observer_elevation, 0.0f};
	
	// Trigger transmittance and multiscattering LUT render
	m_render_transmittance_lut = true;
	m_render_multiscattering_lut = true;
}

void sky_pass::set_atmosphere_upper_limit(float limit)
{
	atmosphere_upper_limit = limit;
	atmosphere_radii[1] = atmosphere_radii[0] + atmosphere_upper_limit;
	atmosphere_radii[3] = atmosphere_radii[1] * atmosphere_radii[1];
	
	// Trigger transmittance and multiscattering LUT render
	m_render_transmittance_lut = true;
	m_render_multiscattering_lut = true;
}

void sky_pass::set_observer_elevation(float elevation)
{
	observer_elevation = elevation;
	observer_position_tween[1] = {0.0f, atmosphere_radii.x() + observer_elevation, 0.0f};
}

void sky_pass::set_rayleigh_parameters(float scale_height, const math::fvec3& scattering)
{
	rayleigh_parameters =
	{
		-1.0f / scale_height,
		scattering.x(),
		scattering.y(),
		scattering.z()
	};
	
	// Trigger transmittance and multiscattering LUT render
	m_render_transmittance_lut = true;
	m_render_multiscattering_lut = true;
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
	
	// Trigger transmittance and multiscattering LUT render
	m_render_transmittance_lut = true;
	m_render_multiscattering_lut = true;
}

void sky_pass::set_ozone_parameters(float lower_limit, float upper_limit, float mode, const math::fvec3& absorption)
{
	ozone_distribution =
	{
		1.0f / (lower_limit - mode),
		1.0f / (upper_limit - mode),
		mode
	};
	ozone_absorption = absorption;
	
	// Trigger transmittance and multiscattering LUT render
	m_render_transmittance_lut = true;
	m_render_multiscattering_lut = true;
}

void sky_pass::set_airglow_luminance(const math::fvec3& luminance)
{
	airglow_luminance = luminance;
}

void sky_pass::set_ground_albedo(const math::fvec3& albedo)
{
	m_ground_albedo = albedo;
	
	// Trigger multiscattering LUT render
	m_render_multiscattering_lut = true;
}

void sky_pass::set_moon_position(const math::fvec3& position)
{
	moon_position_tween[1] = position;
}

void sky_pass::set_moon_rotation(const math::fquat& rotation)
{
	moon_rotation_tween[1] = rotation;
}

void sky_pass::set_moon_angular_radius(float angular_radius)
{
	moon_angular_radius_tween[1] = angular_radius;
}

void sky_pass::set_moon_sunlight_direction(const math::fvec3& direction)
{
	moon_sunlight_direction_tween[1] = direction;
}

void sky_pass::set_moon_sunlight_illuminance(const math::fvec3& illuminance)
{
	moon_sunlight_illuminance_tween[1] = illuminance;
}

void sky_pass::set_moon_planetlight_direction(const math::fvec3& direction)
{
	moon_planetlight_direction_tween[1] = direction;
}

void sky_pass::set_moon_planetlight_illuminance(const math::fvec3& illuminance)
{
	moon_planetlight_illuminance_tween[1] = illuminance;
}

void sky_pass::set_moon_illuminance(const math::fvec3& illuminance, const math::fvec3& transmitted_illuminance)
{
	moon_illuminance_tween[1] = illuminance;
	moon_transmitted_illuminance = transmitted_illuminance;
}

void sky_pass::set_sky_probe(std::shared_ptr<scene::light_probe> probe)
{
	m_sky_probe = probe;
	
	if (m_sky_probe && m_sky_probe->get_luminance_texture())
	{
		auto& luminance_texture = *m_sky_probe->get_luminance_texture();
		
		std::uint16_t face_size = luminance_texture.get_face_size();
		const std::uint8_t mip_count = static_cast<std::uint8_t>(std::bit_width(face_size));
		
		m_sky_probe_framebuffers.resize(mip_count);
		for (std::uint8_t i = 0; i < mip_count; ++i)
		{
			m_sky_probe_framebuffers[i] = std::make_unique<gl::framebuffer>(face_size, face_size);
			m_sky_probe_framebuffers[i]->attach(gl::framebuffer_attachment_type::color, &luminance_texture, i);
			face_size >>= 1;
		}
	}
	else
	{
		m_sky_probe_framebuffers.clear();
	}
	
	rebuild_sky_probe_command_buffer();
}

void sky_pass::rebuild_transmittance_lut_shader_program()
{
	m_transmittance_lut_shader_program = m_transmittance_lut_shader_template->build
	(
		{
			{"SAMPLE_COUNT", std::to_string(m_transmittance_lut_sample_count)}
		}
	);
	if (!m_transmittance_lut_shader_program->linked())
	{
		debug::log::error("Failed to build sky transmittance LUT shader program: {}", m_transmittance_lut_shader_program->info());
		debug::log::warning("{}", m_transmittance_lut_shader_template->configure(gl::shader_stage::vertex));
	}
}

void sky_pass::rebuild_transmittance_lut_command_buffer()
{
	m_transmittance_lut_command_buffer.clear();
	
	if (!m_transmittance_lut_shader_program->linked() || !m_transmittance_lut_texture)
	{
		return;
	}
	
	// Bind framebuffer and shader program
	m_transmittance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->set_viewport(0, 0, m_transmittance_lut_resolution.x(), m_transmittance_lut_resolution.y());
			rasterizer->use_framebuffer(*m_transmittance_lut_framebuffer);
			rasterizer->use_program(*m_transmittance_lut_shader_program);
		}
	);
	
	// Update shader variables
	if (auto atmosphere_radii_var = m_transmittance_lut_shader_program->variable("atmosphere_radii"))
	{
		m_transmittance_lut_command_buffer.emplace_back([&, atmosphere_radii_var](){atmosphere_radii_var->update(atmosphere_radii);});
	}
	if (auto rayleigh_parameters_var = m_transmittance_lut_shader_program->variable("rayleigh_parameters"))
	{
		m_transmittance_lut_command_buffer.emplace_back([&, rayleigh_parameters_var](){rayleigh_parameters_var->update(rayleigh_parameters);});
	}
	if (auto mie_parameters_var = m_transmittance_lut_shader_program->variable("mie_parameters"))
	{
		m_transmittance_lut_command_buffer.emplace_back([&, mie_parameters_var](){mie_parameters_var->update(mie_parameters);});
	}
	if (auto ozone_distribution_var = m_transmittance_lut_shader_program->variable("ozone_distribution"))
	{
		m_transmittance_lut_command_buffer.emplace_back([&, ozone_distribution_var](){ozone_distribution_var->update(ozone_distribution);});
	}
	if (auto ozone_absorption_var = m_transmittance_lut_shader_program->variable("ozone_absorption"))
	{
		m_transmittance_lut_command_buffer.emplace_back([&, ozone_absorption_var](){ozone_absorption_var->update(ozone_absorption);});
	}
	if (auto resolution_var = m_transmittance_lut_shader_program->variable("resolution"))
	{
		m_transmittance_lut_command_buffer.emplace_back([&, resolution_var](){resolution_var->update(math::fvec2(m_transmittance_lut_resolution));});
	}
	
	// Draw quad
	m_transmittance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangle_strip, 0, 4);
		}
	);
}

void sky_pass::rebuild_multiscattering_lut_shader_program()
{
	m_multiscattering_lut_shader_program = m_multiscattering_lut_shader_template->build
	(
		{
			{"DIRECTION_SAMPLE_COUNT", std::to_string(m_multiscattering_lut_direction_sample_count)},
			{"SCATTER_SAMPLE_COUNT", std::to_string(m_multiscattering_lut_scatter_sample_count)}
		}
	);
	if (!m_multiscattering_lut_shader_program->linked())
	{
		debug::log::error("Failed to build sky multiscattering LUT shader program: {}", m_multiscattering_lut_shader_program->info());
		debug::log::warning("{}", m_multiscattering_lut_shader_template->configure(gl::shader_stage::vertex));
	}
}

void sky_pass::rebuild_multiscattering_lut_command_buffer()
{
	m_multiscattering_lut_command_buffer.clear();
	
	if (!m_multiscattering_lut_shader_program->linked() || !m_multiscattering_lut_texture)
	{
		return;
	}
	
	// Bind framebuffer and shader program
	m_multiscattering_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->set_viewport(0, 0, m_multiscattering_lut_resolution.x(), m_multiscattering_lut_resolution.y());
			rasterizer->use_framebuffer(*m_multiscattering_lut_framebuffer);
			rasterizer->use_program(*m_multiscattering_lut_shader_program);
		}
	);
	
	// Update shader variables
	if (auto atmosphere_radii_var = m_multiscattering_lut_shader_program->variable("atmosphere_radii"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, atmosphere_radii_var](){atmosphere_radii_var->update(atmosphere_radii);});
	}
	if (auto rayleigh_parameters_var = m_multiscattering_lut_shader_program->variable("rayleigh_parameters"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, rayleigh_parameters_var](){rayleigh_parameters_var->update(rayleigh_parameters);});
	}
	if (auto mie_parameters_var = m_multiscattering_lut_shader_program->variable("mie_parameters"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, mie_parameters_var](){mie_parameters_var->update(mie_parameters);});
	}
	if (auto ozone_distribution_var = m_multiscattering_lut_shader_program->variable("ozone_distribution"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, ozone_distribution_var](){ozone_distribution_var->update(ozone_distribution);});
	}
	if (auto ozone_absorption_var = m_multiscattering_lut_shader_program->variable("ozone_absorption"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, ozone_absorption_var](){ozone_absorption_var->update(ozone_absorption);});
	}
	if (auto ground_albedo_var = m_multiscattering_lut_shader_program->variable("ground_albedo"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, ground_albedo_var](){ground_albedo_var->update(m_ground_albedo);});
	}
	if (auto resolution_var = m_multiscattering_lut_shader_program->variable("resolution"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, resolution_var](){resolution_var->update(math::fvec2(m_multiscattering_lut_resolution));});
	}
	if (auto transmittance_lut_var = m_multiscattering_lut_shader_program->variable("transmittance_lut"))
	{
		m_multiscattering_lut_command_buffer.emplace_back([&, transmittance_lut_var](){transmittance_lut_var->update(*m_transmittance_lut_texture);});
	}
	
	// Draw quad
	m_multiscattering_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangle_strip, 0, 4);
		}
	);
}

void sky_pass::rebuild_luminance_lut_shader_program()
{
	m_luminance_lut_shader_program = m_luminance_lut_shader_template->build
	(
		{
			{"SAMPLE_COUNT", std::to_string(m_luminance_lut_sample_count)}
		}
	);
	if (!m_luminance_lut_shader_program->linked())
	{
		debug::log::error("Failed to build sky luminance LUT shader program: {}", m_luminance_lut_shader_program->info());
		debug::log::warning("{}", m_luminance_lut_shader_template->configure(gl::shader_stage::vertex));
	}
}

void sky_pass::rebuild_luminance_lut_command_buffer()
{
	m_luminance_lut_command_buffer.clear();
	
	if (!m_luminance_lut_shader_program->linked() || !m_luminance_lut_texture)
	{
		return;
	}
	
	// Bind framebuffer and shader program
	m_luminance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->set_viewport(0, 0, m_luminance_lut_resolution.x(), m_luminance_lut_resolution.y());
			rasterizer->use_framebuffer(*m_luminance_lut_framebuffer);
			rasterizer->use_program(*m_luminance_lut_shader_program);
		}
	);
	
	// Update shader variables
	if (auto light_direction_var = m_luminance_lut_shader_program->variable("light_direction"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, light_direction_var](){light_direction_var->update(dominant_light_direction);});
	}
	if (auto light_illuminance_var = m_luminance_lut_shader_program->variable("light_illuminance"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, light_illuminance_var](){light_illuminance_var->update(dominant_light_illuminance);});
	}
	if (auto atmosphere_radii_var = m_luminance_lut_shader_program->variable("atmosphere_radii"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, atmosphere_radii_var](){atmosphere_radii_var->update(atmosphere_radii);});
	}
	if (auto observer_position_var = m_luminance_lut_shader_program->variable("observer_position"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, observer_position_var](){observer_position_var->update(observer_position);});
	}
	if (auto rayleigh_parameters_var = m_luminance_lut_shader_program->variable("rayleigh_parameters"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, rayleigh_parameters_var](){rayleigh_parameters_var->update(rayleigh_parameters);});
	}
	if (auto mie_parameters_var = m_luminance_lut_shader_program->variable("mie_parameters"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, mie_parameters_var](){mie_parameters_var->update(mie_parameters);});
	}
	if (auto ozone_distribution_var = m_luminance_lut_shader_program->variable("ozone_distribution"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, ozone_distribution_var](){ozone_distribution_var->update(ozone_distribution);});
	}
	if (auto ozone_absorption_var = m_luminance_lut_shader_program->variable("ozone_absorption"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, ozone_absorption_var](){ozone_absorption_var->update(ozone_absorption);});
	}
	if (auto airglow_luminance_var = m_luminance_lut_shader_program->variable("airglow_luminance"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, airglow_luminance_var](){airglow_luminance_var->update(airglow_luminance * camera_exposure);});
	}
	if (auto resolution_var = m_luminance_lut_shader_program->variable("resolution"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, resolution_var](){resolution_var->update(math::fvec2(m_luminance_lut_resolution));});
	}
	if (auto transmittance_lut_var = m_luminance_lut_shader_program->variable("transmittance_lut"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, transmittance_lut_var](){transmittance_lut_var->update(*m_transmittance_lut_texture);});
	}
	if (auto multiscattering_lut_var = m_luminance_lut_shader_program->variable("multiscattering_lut"))
	{
		m_luminance_lut_command_buffer.emplace_back([&, multiscattering_lut_var](){multiscattering_lut_var->update(*m_multiscattering_lut_texture);});
	}
	
	// Draw quad
	m_luminance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangle_strip, 0, 4);
		}
	);
}

void sky_pass::rebuild_sky_probe_command_buffer()
{
	m_sky_probe_command_buffer.clear();
	
	if (!m_sky_probe_shader_program->linked() || m_sky_probe_framebuffers.empty())
	{
		return;
	}
	
	// Bind sky probe framebuffer and shader program
	m_sky_probe_command_buffer.emplace_back
	(
		[&]()
		{
			const auto resolution = m_sky_probe->get_luminance_texture()->get_face_size();
			rasterizer->set_viewport(0, 0, resolution, resolution);
			rasterizer->use_framebuffer(*m_sky_probe_framebuffers[0]);
			rasterizer->use_program(*m_sky_probe_shader_program);
		}
	);
	
	if (auto luminance_lut_var = m_sky_probe_shader_program->variable("luminance_lut"))
	{
		m_sky_probe_command_buffer.emplace_back([&, luminance_lut_var](){luminance_lut_var->update(*m_luminance_lut_texture);});
	}
	if (auto light_direction_var = m_sky_probe_shader_program->variable("light_direction"))
	{
		m_sky_probe_command_buffer.emplace_back([&, light_direction_var](){light_direction_var->update(dominant_light_direction);});
	}
	if (auto light_illuminance_var = m_sky_probe_shader_program->variable("light_illuminance"))
	{
		m_sky_probe_command_buffer.emplace_back([&, light_illuminance_var](){light_illuminance_var->update(dominant_light_illuminance);});
	}
	if (auto observer_position_var = m_sky_probe_shader_program->variable("observer_position"))
	{
		m_sky_probe_command_buffer.emplace_back([&, observer_position_var](){observer_position_var->update(observer_position);});
	}
	if (auto atmosphere_radii_var = m_sky_probe_shader_program->variable("atmosphere_radii"))
	{
		m_sky_probe_command_buffer.emplace_back([&, atmosphere_radii_var](){atmosphere_radii_var->update(atmosphere_radii);});
	}
	if (auto ground_albedo_var = m_sky_probe_shader_program->variable("ground_albedo"))
	{
		m_sky_probe_command_buffer.emplace_back([&, ground_albedo_var](){ground_albedo_var->update(m_ground_albedo);});
	}
	
	// Draw point
	m_sky_probe_command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::points, 0, 1);
			m_sky_probe->set_luminance_outdated(true);
			m_sky_probe->set_illuminance_outdated(true);
		}
	);
}

} // namespace render

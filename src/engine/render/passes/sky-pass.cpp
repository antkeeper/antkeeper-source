// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/passes/sky-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture.hpp>
#include <engine/render/vertex-attribute-location.hpp>
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

namespace render {

sky_pass::sky_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(pipeline, framebuffer),
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
	// Construct LUT sampler
	m_lut_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_edge,
		gl::sampler_address_mode::clamp_to_edge
	);
	
	// Construct empty vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>();
	
	// Transmittance LUT
	{
		// Construct transmittance LUT texture and framebuffer
		rebuild_transmittance_lut_framebuffer();
		
		// Load transmittance LUT shader template
		m_transmittance_lut_shader_template = resource_manager->load<gl::shader_template>("sky-transmittance-lut.glsl");
		
		// Build transmittance LUT shader program
		rebuild_transmittance_lut_shader_program();
		
		// Build transmittance LUT command buffer
		rebuild_transmittance_lut_command_buffer();
	}
	
	// Multiscattering LUT
	{
		// Construct multiscattering LUT texture and framebuffer
		rebuild_multiscattering_lut_framebuffer();
		
		// Load multiscattering LUT shader template
		m_multiscattering_lut_shader_template = resource_manager->load<gl::shader_template>("sky-multiscattering-lut.glsl");
		
		// Build multiscattering LUT shader program
		rebuild_multiscattering_lut_shader_program();
		
		// Build multiscattering LUT command buffer
		rebuild_multiscattering_lut_command_buffer();
	}
	
	// Luminance LUT
	{
		// Construct luminance LUT texture and framebuffer
		rebuild_luminance_lut_framebuffer();
		
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
		debug::log_error("Failed to build sky probe shader program: {}", m_sky_probe_shader_program->info());
		debug::log_warning("{}", m_sky_probe_shader_template->configure(gl::shader_stage::vertex));
	}
	
	// Load moon textures
	m_moon_albedo_map = resource_manager->load<gl::texture_2d>("moon-albedo.tex");
	m_moon_normal_map = resource_manager->load<gl::texture_2d>("moon-normal.tex");
}

void sky_pass::render(render::context& ctx)
{
	if (!(m_layer_mask & ctx.camera->get_layer_mask()))
	{
		return;
	}
	
	m_pipeline->set_color_blend_enabled(false);
	m_pipeline->set_depth_test_enabled(false);
	m_pipeline->set_cull_mode(gl::cull_mode::back);
	
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
	math::fvec3 model_scale = math::fvec3{1.0f, 1.0f, 1.0f} * camera.get_clip_near() * 2.0f;
	math::fmat4 model = math::scale(model_scale);
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
	
	m_pipeline->bind_framebuffer(m_framebuffer);
	m_pipeline->clear_attachments(gl::color_clear_bit | gl::depth_clear_bit | gl::stencil_clear_bit, {});
	
	// Check if any corner of the view frustum is looking at or above the horizon
	bool sky_visible =
		(camera.pick(math::fvec2{-1,  1}).direction.y() > 0.0f) ||
		(camera.pick(math::fvec2{-1, -1}).direction.y() > 0.0f) ||
		(camera.pick(math::fvec2{ 1,  1}).direction.y() > 0.0f) ||
		(camera.pick(math::fvec2{ 1, -1}).direction.y() > 0.0f);
	if (!sky_visible)
	{
		// Sky not visible, abort
		return;
	}
	
	const auto& viewport_dimensions = (m_framebuffer) ? m_framebuffer->dimensions() : m_pipeline->get_default_framebuffer_dimensions();
	const gl::viewport viewport[1] =
	{{
		0.0f,
		0.0f,
		static_cast<float>(viewport_dimensions[0]),
		static_cast<float>(viewport_dimensions[1])
	}};
	m_pipeline->set_viewport(0, viewport);
	math::fvec2 resolution = {viewport[0].width, viewport[0].height};
	
	// Draw atmosphere
	if (sky_model && sky_shader_program)
	{
		m_pipeline->bind_shader_program(sky_shader_program.get());
		
		// Upload shader parameters
		if (model_view_projection_var)
			model_view_projection_var->update(model_view_projection);
		if (view_var)
		{
			view_var->update(view);
		}
		if (view_projection_var)
		{
			view_projection_var->update(view_projection);
		}
		if (inv_view_projection_var)
		{
			const auto inv_view_projection = math::fmat4(math::fmat3(camera.get_inv_view())) * camera.get_inv_projection();
			inv_view_projection_var->update(inv_view_projection);
			// inv_view_projection_var->update(camera.get_inv_view_projection());
		}
		if (camera_position_var)
		{
			camera_position_var->update(camera.get_translation());
		}
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
		
		m_pipeline->set_primitive_topology(sky_model_primitive_topology);
		m_pipeline->bind_vertex_array(sky_model_vao);
		m_pipeline->bind_vertex_buffers(0, {&sky_model_vbo, 1}, {&sky_model_vertex_offset, 1}, {&sky_model_vertex_stride, 1});
		m_pipeline->draw(sky_model_vertex_count, 1, sky_model_first_vertex, 0);
	}
	
	// Enable additive blending
	m_pipeline->set_color_blend_enabled(true);
	m_pipeline->set_color_blend_equation
	({
		gl::blend_factor::one,
		gl::blend_factor::one,
		gl::blend_op::add,
		gl::blend_factor::one,
		gl::blend_factor::one,
		gl::blend_op::add
	});
	
	// Flag moon pixels in stencil buffer
	m_pipeline->set_stencil_test_enabled(true);
	m_pipeline->set_stencil_write_mask(gl::stencil_face_front_and_back, 0xff);
	m_pipeline->set_stencil_reference(gl::stencil_face_front_and_back, 1);
	m_pipeline->set_stencil_compare_mask(gl::stencil_face_front_and_back, 0xff);
	m_pipeline->set_stencil_op
	(
		gl::stencil_face_front_and_back,
		gl::stencil_op::keep,
		gl::stencil_op::replace,
		gl::stencil_op::keep,
		gl::compare_op::always
	);
	
	// Draw moon model
	//if (moon_position.y() >= -moon_angular_radius)
	if (moon_shader_program)
	{
		float moon_distance = camera.get_clip_near() * 2.0f;
		float moon_radius = moon_angular_radius * moon_distance;
		
		math::transform<float> moon_transform;
		moon_transform.translation = math::normalize(moon_position) * moon_distance;
		moon_transform.rotation = moon_rotation_tween.interpolate(ctx.alpha);
		moon_transform.scale = {moon_radius, moon_radius, moon_radius};
		
		model = moon_transform.matrix();		
		math::fmat3 normal_model = math::transpose(math::inverse(math::fmat3(model)));
		
		m_pipeline->bind_shader_program(moon_shader_program.get());
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
		
		m_pipeline->set_primitive_topology(moon_model_primitive_topology);
		m_pipeline->bind_vertex_array(moon_model_vao);
		m_pipeline->bind_vertex_buffers(0, {&moon_model_vbo, 1}, {&moon_model_vertex_offset, 1}, {&moon_model_vertex_stride, 1});
		m_pipeline->draw(moon_model_vertex_count, 1, moon_model_first_vertex, 0);
	}
	
	// Prevent stars from being drawn in front of the moon
	m_pipeline->set_stencil_compare_mask(gl::stencil_face_front_and_back, 0x00);
	m_pipeline->set_stencil_op
	(
		gl::stencil_face_front_and_back,
		gl::stencil_op::keep,
		gl::stencil_op::replace,
		gl::stencil_op::keep,
		gl::compare_op::not_equal
	);
	
	// Draw stars
	if (star_shader_program)
	{
		float star_distance = camera.get_clip_near() * 2.0f;
		
		model = math::fmat4(math::fmat3(icrf_to_eus.r)) * math::scale(math::fvec3{star_distance, star_distance, star_distance});
		
		model_view_projection = view_projection * model;
		
		m_pipeline->bind_shader_program(star_shader_program.get());
		if (star_model_view_projection_var)
			star_model_view_projection_var->update(model_view_projection);
		if (star_exposure_var)
			star_exposure_var->update(camera_exposure);
		if (star_inv_resolution_var)
			star_inv_resolution_var->update(1.0f / resolution);
		
		m_pipeline->set_primitive_topology(stars_model_primitive_topology);
		m_pipeline->bind_vertex_array(stars_model_vao);
		m_pipeline->bind_vertex_buffers(0, {&stars_model_vbo, 1}, {&stars_model_vertex_offset, 1}, {&stars_model_vertex_stride, 1});
		m_pipeline->draw(stars_model_vertex_count, 1, stars_model_first_vertex, 0);
	}
	
	m_pipeline->set_stencil_test_enabled(false);
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
		
		rebuild_transmittance_lut_framebuffer();
		
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
		
		rebuild_multiscattering_lut_framebuffer();
		
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
		
		rebuild_luminance_lut_framebuffer();
		
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
		sky_model_vbo = model->get_vertex_buffer().get();
		
		for (const auto& group: model->get_groups())
		{
			sky_model_primitive_topology = group.primitive_topology;
			sky_model_first_vertex = group.first_vertex;
			sky_model_vertex_count = group.vertex_count;
			sky_material = group.material.get();
		}
		sky_model_vertex_offset = sky_model->get_vertex_offset();
		sky_model_vertex_stride = sky_model->get_vertex_stride();
		
		if (sky_material)
		{
			sky_shader_program = sky_material->get_shader_template()->build();
			
			if (sky_shader_program->linked())
			{
				model_view_projection_var = sky_shader_program->variable("model_view_projection");
				view_var = sky_shader_program->variable("view");
				view_projection_var = sky_shader_program->variable("view_projection");
				inv_view_projection_var = sky_shader_program->variable("inv_view_projection");
				camera_position_var = sky_shader_program->variable("camera_position");
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
				debug::log_error("Failed to build sky shader program: {}", sky_shader_program->info());
				debug::log_warning("{}", sky_material->get_shader_template()->configure(gl::shader_stage::vertex));
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
		moon_model_vbo = model->get_vertex_buffer().get();
		
		for (const auto& group: model->get_groups())
		{
			moon_model_primitive_topology = group.primitive_topology;
			moon_model_first_vertex = group.first_vertex;
			moon_model_vertex_count = group.vertex_count;
			moon_material = group.material.get();
		}
		moon_model_vertex_offset = moon_model->get_vertex_offset();
		moon_model_vertex_stride = moon_model->get_vertex_stride();
		
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
				debug::log_error("Failed to build moon shader program: {}", moon_shader_program->info());
				debug::log_warning("{}", moon_material->get_shader_template()->configure(gl::shader_stage::vertex));
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
		stars_model_vbo = model->get_vertex_buffer().get();
		
		for (const auto& group: model->get_groups())
		{
			stars_model_primitive_topology = group.primitive_topology;
			stars_model_first_vertex = group.first_vertex;
			stars_model_vertex_count = group.vertex_count;
			star_material = group.material.get();
		}
		stars_model_vertex_offset = stars_model->get_vertex_offset();
		stars_model_vertex_stride = stars_model->get_vertex_stride();
		
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
				debug::log_error("Failed to build star shader program: {}", star_shader_program->info());
				debug::log_warning("{}", star_material->get_shader_template()->configure(gl::shader_stage::vertex));
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
		const auto& luminance_texture = m_sky_probe->get_luminance_texture();
		
		const auto face_size = luminance_texture->get_image_view()->get_image()->get_dimensions()[0];
		const auto mip_count = static_cast<std::uint32_t>(std::bit_width(face_size));
		
		m_sky_probe_framebuffers.resize(mip_count);
		for (std::uint32_t i = 0; i < mip_count; ++i)
		{
			const gl::framebuffer_attachment attachments[1] =
			{{
				gl::color_attachment_bit,
				luminance_texture->get_image_view(),
				i
			}};
			m_sky_probe_framebuffers[i] = std::make_unique<gl::framebuffer>(attachments, face_size >> i, face_size >> i);
		}
	}
	else
	{
		m_sky_probe_framebuffers.clear();
	}
	
	rebuild_sky_probe_command_buffer();
}

void sky_pass::rebuild_transmittance_lut_framebuffer()
{
	// Rebuild transmittance LUT texture
	m_transmittance_lut_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r32g32b32_sfloat,
				m_transmittance_lut_resolution.x(),
				m_transmittance_lut_resolution.y()
			)
		),
		m_lut_sampler
	);
	
	// Rebuild transmittance LUT framebuffer
	const gl::framebuffer_attachment attachments[1] =
	{{
		gl::color_attachment_bit,
		m_transmittance_lut_texture->get_image_view(),
		0
	}};
	m_transmittance_lut_framebuffer = std::make_shared<gl::framebuffer>(attachments, m_transmittance_lut_resolution.x(), m_transmittance_lut_resolution.y());
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
		debug::log_error("Failed to build sky transmittance LUT shader program: {}", m_transmittance_lut_shader_program->info());
		debug::log_warning("{}", m_transmittance_lut_shader_template->configure(gl::shader_stage::vertex));
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
			const gl::viewport viewport[1] =
			{{
				0.0f,
				0.0f,
				static_cast<float>(m_transmittance_lut_resolution.x()),
				static_cast<float>(m_transmittance_lut_resolution.y())
			}};
			m_pipeline->set_viewport(0, viewport);
			
			m_pipeline->bind_framebuffer(m_transmittance_lut_framebuffer.get());
			m_pipeline->bind_shader_program(m_transmittance_lut_shader_program.get());
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
	
	m_transmittance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			// Draw fullscreen triangle
			m_pipeline->bind_vertex_array(m_vertex_array.get());
			m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
			m_pipeline->draw(3, 1, 0, 0);
		}
	);
}

void sky_pass::rebuild_multiscattering_lut_framebuffer()
{
	// Rebuild multiscattering LUT texture
	m_multiscattering_lut_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r32g32b32_sfloat,
				m_multiscattering_lut_resolution.x(),
				m_multiscattering_lut_resolution.y()
			)
		),
		m_lut_sampler
	);
	
	// Rebuild multiscattering LUT framebuffer and attach texture
	const gl::framebuffer_attachment attachments[1] =
	{{
		gl::color_attachment_bit,
		m_multiscattering_lut_texture->get_image_view(),
		0
	}};
	m_multiscattering_lut_framebuffer = std::make_shared<gl::framebuffer>(attachments, m_multiscattering_lut_resolution.x(), m_multiscattering_lut_resolution.y());
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
		debug::log_error("Failed to build sky multiscattering LUT shader program: {}", m_multiscattering_lut_shader_program->info());
		debug::log_warning("{}", m_multiscattering_lut_shader_template->configure(gl::shader_stage::vertex));
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
			const gl::viewport viewport[1] =
			{{
				0.0f,
				0.0f,
				static_cast<float>(m_multiscattering_lut_resolution.x()),
				static_cast<float>(m_multiscattering_lut_resolution.y())
			}};
			m_pipeline->set_viewport(0, viewport);
			
			m_pipeline->bind_framebuffer(m_multiscattering_lut_framebuffer.get());
			m_pipeline->bind_shader_program(m_multiscattering_lut_shader_program.get());
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
	
	m_multiscattering_lut_command_buffer.emplace_back
	(
		[&]()
		{
			// Draw fullscreen triangle
			m_pipeline->bind_vertex_array(m_vertex_array.get());
			m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
			m_pipeline->draw(3, 1, 0, 0);
		}
	);
}

void sky_pass::rebuild_luminance_lut_framebuffer()
{
	// Rebuild luminance LUT texture
	m_luminance_lut_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r32g32b32_sfloat,
				m_luminance_lut_resolution.x(),
				m_luminance_lut_resolution.y()
			)
		),
		m_lut_sampler
	);
	
	// Rebuild luminance LUT framebuffer
	const gl::framebuffer_attachment attachments[1] =
	{{
		gl::color_attachment_bit,
		m_luminance_lut_texture->get_image_view(),
		0
	}};
	m_luminance_lut_framebuffer = std::make_shared<gl::framebuffer>(attachments, m_luminance_lut_resolution.x(), m_luminance_lut_resolution.y());
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
		debug::log_error("Failed to build sky luminance LUT shader program: {}", m_luminance_lut_shader_program->info());
		debug::log_warning("{}", m_luminance_lut_shader_template->configure(gl::shader_stage::vertex));
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
			const gl::viewport viewport[1] =
			{{
				0.0f,
				0.0f,
				static_cast<float>(m_luminance_lut_resolution.x()),
				static_cast<float>(m_luminance_lut_resolution.y())
			}};
			m_pipeline->set_viewport(0, viewport);
			
			m_pipeline->bind_framebuffer(m_luminance_lut_framebuffer.get());
			m_pipeline->bind_shader_program(m_luminance_lut_shader_program.get());
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
	
	m_luminance_lut_command_buffer.emplace_back
	(
		[&]()
		{
			// Draw fullscreen triangle
			m_pipeline->bind_vertex_array(m_vertex_array.get());
			m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
			m_pipeline->draw(3, 1, 0, 0);
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
			const auto resolution = m_sky_probe->get_luminance_texture()->get_image_view()->get_image()->get_dimensions()[0];
			const gl::viewport viewport[1] =
			{{
				0.0f,
				0.0f,
				static_cast<float>(resolution),
				static_cast<float>(resolution)
			}};
			m_pipeline->set_viewport(0, viewport);
			
			m_pipeline->bind_framebuffer(m_sky_probe_framebuffers[0].get());
			m_pipeline->bind_shader_program(m_sky_probe_shader_program.get());
			m_pipeline->bind_vertex_array(m_vertex_array.get());
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
			m_pipeline->set_primitive_topology(gl::primitive_topology::point_list);
			m_pipeline->draw(1, 1, 0, 0);
			m_sky_probe->set_luminance_outdated(true);
			m_sky_probe->set_illuminance_outdated(true);
		}
	);
}

} // namespace render

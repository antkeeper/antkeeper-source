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

#ifndef ANTKEEPER_RENDER_SKY_PASS_HPP
#define ANTKEEPER_RENDER_SKY_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/animation/tween.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/math/se3.hpp>
#include <engine/scene/object.hpp>

class resource_manager;

namespace render {

class material;
class model;

/**
 *
 */
class sky_pass: public pass
{
public:
	sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~sky_pass() = default;
	void render(const render::context& ctx, render::queue& queue) override;
	
	void update_tweens();
	
	void set_magnification(float scale);
	
	void set_sky_model(std::shared_ptr<render::model> model);
	void set_moon_model(std::shared_ptr<render::model> model);
	void set_stars_model(std::shared_ptr<render::model> model);
	
	void set_icrf_to_eus(const math::transformation::se3<float>& transformation);
	
	void set_sun_position(const float3& position);
	void set_sun_luminance(const float3& luminance);
	void set_sun_illuminance(const float3& illuminance, const float3& transmitted_illuminance);
	void set_sun_angular_radius(float radius);
	void set_planet_radius(float radius);
	void set_atmosphere_upper_limit(float limit);
	void set_observer_elevation(float elevation);
	void set_rayleigh_parameters(float scale_height, const float3& scattering);
	void set_mie_parameters(float scale_height, float scattering, float extinction, float anisotropy);
	void set_ozone_parameters(float lower_limit, float upper_limit, float mode, const float3& absorption);
	void set_airglow_illuminance(const float3& illuminance);
	
	void set_moon_position(const float3& position);
	void set_moon_rotation(const math::quaternion<float>& rotation);
	void set_moon_angular_radius(float angular_radius);
	void set_moon_sunlight_direction(const float3& direction);
	void set_moon_sunlight_illuminance(const float3& illuminance);
	void set_moon_planetlight_direction(const float3& direction);
	void set_moon_planetlight_illuminance(const float3& illuminance);
	void set_moon_illuminance(const float3& illuminance, const float3& transmitted_illuminance);
	
	/**
	 * Sets the resolution of transmittance LUT.
	 *
	 * @param width Transmittance LUT width, in pixels.
	 * @param height Transmittance LUT height, in pixels.
	 */
	void set_transmittance_lut_resolution(std::uint16_t width, std::uint16_t height);

private:
	void rebuild_transmittance_lut_command_buffer();
	void rebuild_sky_lut_command_buffer();
	
	std::unique_ptr<gl::vertex_buffer> quad_vbo;
	std::unique_ptr<gl::vertex_array> quad_vao;
	
	std::unique_ptr<gl::texture_2d> transmittance_lut_texture;
	std::unique_ptr<gl::framebuffer> transmittance_lut_framebuffer;
	float2 transmittance_lut_resolution;
	std::shared_ptr<gl::shader_template> transmittance_lut_shader_template;
	std::unique_ptr<gl::shader_program> transmittance_lut_shader_program;
	bool render_transmittance_lut;
	std::vector<std::function<void()>> transmittance_lut_command_buffer;
	
	std::unique_ptr<gl::texture_2d> sky_lut_texture;
	std::unique_ptr<gl::framebuffer> sky_lut_framebuffer;
	std::shared_ptr<gl::shader_template> sky_lut_shader_template;
	std::unique_ptr<gl::shader_program> sky_lut_shader_program;
	float2 sky_lut_resolution;
	std::vector<std::function<void()>> sky_lut_command_buffer;
	
	float3 dominant_light_direction;
	float3 dominant_light_illuminance;
	float3 observer_position;
	float camera_exposure;

	std::shared_ptr<gl::shader_program> sky_shader_program;
	const gl::shader_variable* model_view_projection_var;
	const gl::shader_variable* mouse_var;
	const gl::shader_variable* resolution_var;
	const gl::shader_variable* light_direction_var;
	const gl::shader_variable* sun_luminance_var;
	const gl::shader_variable* sun_angular_radius_var;
	const gl::shader_variable* atmosphere_radii_var;
	const gl::shader_variable* observer_position_var;
	const gl::shader_variable* sky_illuminance_lut_var;
	const gl::shader_variable* sky_illuminance_lut_resolution_var;
	
	std::shared_ptr<gl::shader_program> moon_shader_program;
	const gl::shader_variable* moon_model_var;
	const gl::shader_variable* moon_view_projection_var;
	const gl::shader_variable* moon_normal_model_var;
	const gl::shader_variable* moon_camera_position_var;
	const gl::shader_variable* moon_sunlight_direction_var;
	const gl::shader_variable* moon_sunlight_illuminance_var;
	const gl::shader_variable* moon_planetlight_direction_var;
	const gl::shader_variable* moon_planetlight_illuminance_var;
	
	std::shared_ptr<render::model> sky_model;
	const material* sky_material;
	const gl::vertex_array* sky_model_vao;
	gl::drawing_mode sky_model_drawing_mode;
	std::size_t sky_model_start_index;
	std::size_t sky_model_index_count;
	
	std::shared_ptr<render::model> moon_model;
	const material* moon_material;
	const gl::vertex_array* moon_model_vao;
	gl::drawing_mode moon_model_drawing_mode;
	std::size_t moon_model_start_index;
	std::size_t moon_model_index_count;
	
	std::shared_ptr<render::model> stars_model;
	const material* star_material;
	const gl::vertex_array* stars_model_vao;
	gl::drawing_mode stars_model_drawing_mode;
	std::size_t stars_model_start_index;
	std::size_t stars_model_index_count;
	std::unique_ptr<gl::shader_program> star_shader_program;
	const gl::shader_variable* star_model_view_var;
	const gl::shader_variable* star_projection_var;
	const gl::shader_variable* star_exposure_var;
	const gl::shader_variable* star_distance_var;

	float2 mouse_position;
	
	tween<float3> sun_position_tween;
	tween<float3> sun_luminance_tween;
	tween<float3> sun_illuminance_tween;
	float3 sun_transmitted_illuminance;
	tween<float3> icrf_to_eus_translation;
	tween<math::quaternion<float>> icrf_to_eus_rotation;
	
	tween<float3> moon_position_tween;
	tween<math::quaternion<float>> moon_rotation_tween;
	tween<float> moon_angular_radius_tween;
	tween<float3> moon_sunlight_direction_tween;
	tween<float3> moon_sunlight_illuminance_tween;
	tween<float3> moon_planetlight_direction_tween;
	tween<float3> moon_planetlight_illuminance_tween;
	tween<float3> moon_illuminance_tween;
	float3 moon_transmitted_illuminance;
	
	float sun_angular_radius;
	float atmosphere_upper_limit;
	float3 atmosphere_radii;
	float observer_elevation;
	tween<float3> observer_position_tween;
	float4 rayleigh_parameters;
	float4 mie_parameters;
	float3 ozone_distribution;
	float3 ozone_absorption;
	float3 airglow_illuminance;
	
	float magnification;
};

} // namespace render

#endif // ANTKEEPER_RENDER_SKY_PASS_HPP

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

#ifndef ANTKEEPER_SKY_PASS_HPP
#define ANTKEEPER_SKY_PASS_HPP

#include "renderer/render-pass.hpp"
#include "utility/fundamental-types.hpp"
#include "event/event-handler.hpp"
#include "event/input-events.hpp"
#include "animation/tween.hpp"
#include "math/quaternion-type.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/texture-2d.hpp"
#include "gl/drawing-mode.hpp"
#include "physics/frame.hpp"
#include "scene/object.hpp"

class resource_manager;
class model;
class material;

/**
 *
 */
class sky_pass: public render_pass,
	public event_handler<mouse_moved_event>
{
public:
	sky_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~sky_pass();
	virtual void render(render_context* context) const final;
	
	void update_tweens();
	
	void set_sky_model(const model* model);
	void set_time_tween(const tween<double>* time);
	void set_moon_model(const model* model);
	
	void set_topocentric_frame(const physics::frame<float>& frame);
	
	void set_sun_position(const float3& position);
	void set_sun_color(const float3& color);
	void set_sun_angular_radius(float radius);
	void set_observer_altitude(float altitude);
	void set_scale_heights(float rayleigh, float mie);
	void set_scattering_coefficients(const float3& r, const float3& m);
	void set_mie_anisotropy(float g);
	void set_atmosphere_radii(float inner, float outer);

private:
	virtual void handle_event(const mouse_moved_event& event);

	gl::shader_program* sky_shader_program;
	const gl::shader_input* model_view_projection_input;
	const gl::shader_input* mouse_input;
	const gl::shader_input* resolution_input;
	const gl::shader_input* time_input;
	const gl::shader_input* exposure_input;
	
	const gl::shader_input* observer_altitude_input;
	const gl::shader_input* sun_direction_input;
	const gl::shader_input* sun_color_input;
	const gl::shader_input* sun_angular_radius_input;
	const gl::shader_input* scale_height_rm_input;
	const gl::shader_input* rayleigh_scattering_input;
	const gl::shader_input* mie_scattering_input;
	const gl::shader_input* mie_anisotropy_input;
	const gl::shader_input* atmosphere_radii_input;
	
	gl::shader_program* moon_shader_program;
	const gl::shader_input* moon_model_view_projection_input;
	const gl::shader_input* moon_normal_model_input;
	const gl::shader_input* moon_moon_position_input;
	const gl::shader_input* moon_sun_position_input;
	
	gl::vertex_buffer* star_catalog_vbo;
	gl::vertex_array* star_catalog_vao;
	gl::shader_program* star_shader_program;
	const gl::shader_input* star_model_view_input;
	const gl::shader_input* star_projection_input;
	const gl::shader_input* star_exposure_input;
	const gl::shader_input* star_distance_input;
	std::size_t star_count;
	
	const model* sky_model;
	const material* sky_material;
	const gl::vertex_array* sky_model_vao;
	gl::drawing_mode sky_model_drawing_mode;
	std::size_t sky_model_start_index;
	std::size_t sky_model_index_count;
	
	const model* moon_model;
	const material* moon_material;
	const gl::vertex_array* moon_model_vao;
	gl::drawing_mode moon_model_drawing_mode;
	std::size_t moon_model_start_index;
	std::size_t moon_model_index_count;

	const gl::texture_2d* blue_noise_map;
	const gl::texture_2d* sky_gradient;
	const gl::texture_2d* sky_gradient2;
	float2 mouse_position;
	
	const tween<double>* time_tween;
	tween<float> observer_altitude_tween;
	tween<float3> sun_position_tween;
	tween<float3> sun_color_tween;
	tween<float3> topocentric_frame_translation;
	tween<math::quaternion<float>> topocentric_frame_rotation;
	
	float sun_angular_radius;
	float2 scale_height_rm;
	float3 rayleigh_scattering;
	float3 mie_scattering;
	float2 mie_anisotropy;
	float3 atmosphere_radii;
};

#endif // ANTKEEPER_SKY_PASS_HPP

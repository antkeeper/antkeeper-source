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
	void set_horizon_color(const float3& color);
	void set_zenith_color(const float3& color);
	void set_time_of_day(float time);
	void set_blue_noise_map(const gl::texture_2d* texture);
	void set_time_tween(const tween<double>* time);
	void set_moon_model(const model* model);
	
	void set_julian_day(float jd);
	void set_observer_location(float latitude, float longitude, float altitude);
	void set_sun_coordinates(const float3& position, const float2& az_el);
	void set_moon_coordinates(const float3& position, const float2& az_el);
	void set_moon_rotation(const math::quaternion<float>& rotation);
	
	void set_moon_angular_radius(float radius);
	void set_sun_angular_radius(float radius);

private:
	virtual void handle_event(const mouse_moved_event& event);

	gl::shader_program* sky_shader_program;
	const gl::shader_input* model_view_projection_input;
	const gl::shader_input* horizon_color_input;
	const gl::shader_input* zenith_color_input;
	const gl::shader_input* mouse_input;
	const gl::shader_input* resolution_input;
	const gl::shader_input* time_input;
	const gl::shader_input* time_of_day_input;
	const gl::shader_input* observer_location_input;
	const gl::shader_input* sun_position_input;
	const gl::shader_input* sun_az_el_input;
	const gl::shader_input* moon_position_input;
	const gl::shader_input* moon_az_el_input;
	const gl::shader_input* blue_noise_map_input;
	const gl::shader_input* julian_day_input;
	const gl::shader_input* cos_sun_angular_radius_input;
	const gl::shader_input* cos_moon_angular_radius_input;
	
	gl::shader_program* moon_shader_program;
	const gl::shader_input* moon_model_view_projection_input;
	const gl::shader_input* moon_normal_model_input;
	const gl::shader_input* moon_moon_position_input;
	const gl::shader_input* moon_sun_position_input;
	
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
	float2 mouse_position;

	const tween<double>* time_tween;
	float3 observer_location;
	tween<float> time_of_day_tween;
	
	tween<float> julian_day_tween;
	tween<float3> sun_position_tween;
	tween<float2> sun_az_el_tween;
	tween<float3> moon_position_tween;
	tween<float2> moon_az_el_tween;
	tween<float3> horizon_color_tween;
	tween<float3> zenith_color_tween;
	
	math::quaternion<float> moon_rotation;
	
	float moon_angular_radius;
	float cos_moon_angular_radius;
	float sun_angular_radius;
	float cos_sun_angular_radius;
};

#endif // ANTKEEPER_SKY_PASS_HPP

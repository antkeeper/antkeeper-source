/*
 * Copyright (C) 2020  Christopher J. Howard
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

class shader_program;
class shader_input;
class vertex_buffer;
class vertex_array;
class texture_2d;
class resource_manager;
class directional_light;

/**
 *
 */
class sky_pass: public render_pass,
	public event_handler<mouse_moved_event>
{
public:
	sky_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~sky_pass();
	virtual void render(render_context* context) const final;
	
	void set_sun_angular_radius(float angle);
	void set_sun_color(const float3& color);
	void set_horizon_color(const float3& color);
	void set_zenith_color(const float3& color);
	void set_sun_light(const directional_light* direction);
	void set_sky_palette(const texture_2d* texture);
	void set_time_tween(const tween<double>* time);

private:
	virtual void handle_event(const mouse_moved_event& event);

	shader_program* shader_program;
	const shader_input* matrix_input;
	const shader_input* sun_direction_input;
	const shader_input* sun_angular_radius_input;
	const shader_input* sun_color_input;
	const shader_input* horizon_color_input;
	const shader_input* zenith_color_input;
	const shader_input* sky_palette_input;
	const shader_input* mouse_input;
	const shader_input* resolution_input;
	const shader_input* time_input;

	vertex_buffer* quad_vbo;
	vertex_array* quad_vao;

	float sun_angular_radius;
	float3 sun_color;
	float3 horizon_color;
	float3 zenith_color;
	const directional_light* sun_light;
	const texture_2d* sky_palette;
	float2 mouse_position;
	const tween<double>* time_tween;
};

#endif // ANTKEEPER_SKY_PASS_HPP

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

class shader_program;
class shader_input;
class vertex_buffer;
class vertex_array;
class texture_2d;
class resource_manager;

/**
 *
 */
class sky_pass: public render_pass
{
public:
	sky_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~sky_pass();
	virtual void render(render_context* context) const final;

private:
	shader_program* shader_program;
	const shader_input* matrix_input;
	const shader_input* sun_direction_input;
	const shader_input* sun_angular_radius_input;
	const shader_input* sky_gradient_input;
	const shader_input* bayer_matrix_input;

	vertex_buffer* quad_vbo;
	vertex_array* quad_vao;
	texture_2d* sky_gradient;
	texture_2d* bayer_matrix;
};

#endif // ANTKEEPER_SKY_PASS_HPP


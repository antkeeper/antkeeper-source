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

#ifndef ANTKEEPER_FINAL_PASS_HPP
#define ANTKEEPER_FINAL_PASS_HPP

#include "renderer/render-pass.hpp"
#include "math/math.hpp"

class shader_program;
class shader_input;
class vertex_buffer;
class vertex_array;
class texture_2d;
class resource_manager;

/**
 *
 */
class final_pass: public render_pass
{
public:
	final_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~final_pass();
	virtual void render(render_context* context) const final;
	
	void set_color_texture(const texture_2d* texture);
	void set_bloom_texture(const texture_2d* texture);

private:
	shader_program* shader_program;
	const shader_input* color_texture_input;
	const shader_input* bloom_texture_input;
	const shader_input* resolution_input;
	vertex_buffer* quad_vbo;
	vertex_array* quad_vao;
	
	const texture_2d* color_texture;
	const texture_2d* bloom_texture;
};

#endif // ANTKEEPER_FINAL_PASS_HPP


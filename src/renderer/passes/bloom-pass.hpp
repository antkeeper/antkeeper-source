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

#ifndef ANTKEEPER_BLOOM_PASS_HPP
#define ANTKEEPER_BLOOM_PASS_HPP

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
class bloom_pass: public render_pass
{
public:
	bloom_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~bloom_pass();
	virtual void render(render_context* context) const final;
	
	void set_source_texture(const texture_2d* texture);
	void set_brightness_threshold(float threshold);
	void set_blur_iterations(int iterations);

private:
	vertex_buffer* quad_vbo;
	vertex_array* quad_vao;
	
	const ::framebuffer* pingpong_framebuffers[2];
	const texture_2d* pingpong_textures[2];
	texture_2d* cloned_framebuffer_texture;
	::framebuffer* cloned_framebuffer;
	
	shader_program* threshold_shader;
	const shader_input* threshold_shader_image_input;
	const shader_input* threshold_shader_resolution_input;
	const shader_input* threshold_shader_threshold_input;
	
	shader_program* blur_shader;
	const shader_input* blur_shader_image_input;
	const shader_input* blur_shader_resolution_input;
	const shader_input* blur_shader_direction_input;
	
	const texture_2d* source_texture;
	float brightness_threshold;
	int blur_iterations;
};

#endif // ANTKEEPER_BLOOM_PASS_HPP


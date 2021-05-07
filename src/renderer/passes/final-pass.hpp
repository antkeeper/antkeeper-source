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

#ifndef ANTKEEPER_FINAL_PASS_HPP
#define ANTKEEPER_FINAL_PASS_HPP

#include "renderer/render-pass.hpp"
#include "math/math.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/texture-2d.hpp"
#include "animation/tween.hpp"

class resource_manager;

/**
 *
 */
class final_pass: public render_pass
{
public:
	final_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~final_pass();
	virtual void render(render_context* context) const final;
	
	void set_color_texture(const gl::texture_2d* texture);
	void set_bloom_texture(const gl::texture_2d* texture);
	void set_blue_noise_texture(const gl::texture_2d* texture);
	void set_time_tween(const tween<double>* time);

private:
	gl::shader_program* shader_program;
	const gl::shader_input* color_texture_input;
	const gl::shader_input* bloom_texture_input;
	const gl::shader_input* blue_noise_texture_input;
	const gl::shader_input* blue_noise_scale_input;
	const gl::shader_input* resolution_input;
	const gl::shader_input* time_input;
	gl::vertex_buffer* quad_vbo;
	gl::vertex_array* quad_vao;
	
	const gl::texture_2d* color_texture;
	const gl::texture_2d* bloom_texture;
	const gl::texture_2d* blue_noise_texture;
	float blue_noise_scale;
	
	const tween<double>* time_tween;
};

#endif // ANTKEEPER_FINAL_PASS_HPP


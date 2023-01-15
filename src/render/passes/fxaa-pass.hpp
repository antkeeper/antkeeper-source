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

#ifndef ANTKEEPER_RENDER_FXAA_PASS_HPP
#define ANTKEEPER_RENDER_FXAA_PASS_HPP

#include "render/pass.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/texture-2d.hpp"

class resource_manager;

namespace render {

/**
 * FXAA render pass.
 *
 * @see Lottes, T. (2009). Fxaa. White paper, Nvidia, Febuary, 2.
 */
class fxaa_pass: public pass
{
public:
	/**
	 * Constructs an FXAA pass.
	 *
	 * @param rasterizer Rasterizer.
	 * @param framebuffer Target framebuffer.
	 * @param resource_manager Resource manager.
	 */
	fxaa_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	
	/**
	 * Destructs an FXAA pass.
	 */
	virtual ~fxaa_pass();
	
	/**
	 * Renders FXAA.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	virtual void render(const render::context& ctx, render::queue& queue) const final;
	
	/**
	 * Sets the FXAA source texture.
	 *
	 * @param texture FXAA source texture.
	 */
	void set_source_texture(const gl::texture_2d* texture);

private:
	const gl::texture_2d* source_texture;
	
	gl::shader_program* shader;
	const gl::shader_input* source_texture_input;
	const gl::shader_input* texel_size_input;
	
	gl::vertex_buffer* quad_vbo;
	gl::vertex_array* quad_vao;
};

} // namespace render

#endif // ANTKEEPER_RENDER_FXAA_PASS_HPP

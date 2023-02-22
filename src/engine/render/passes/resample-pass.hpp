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

#ifndef ANTKEEPER_RENDER_RESAMPLE_PASS_HPP
#define ANTKEEPER_RENDER_RESAMPLE_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/render/shader-template.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-input.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture-2d.hpp>

class resource_manager;

namespace render {

/**
 * Resamples a texture.
 */
class resample_pass: public pass
{
public:
	/**
	 * Constructs a resample pass.
	 *
	 * @param rasterizer Rasterizer.
	 * @param framebuffer Target framebuffer.
	 * @param resource_manager Resource manager.
	 */
	resample_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	
	/**
	 * Destructs a resample pass.
	 */
	virtual ~resample_pass();
	
	/**
	 * Resamples a texture.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	virtual void render(const render::context& ctx, render::queue& queue) const final;
	
	/**
	 * Sets the resample source texture.
	 *
	 * @param texture Texture to resample.
	 */
	void set_source_texture(const gl::texture_2d* texture);

private:
	const gl::texture_2d* source_texture;
	
	render::shader_template* shader_template;
	gl::shader_program* shader;
	const gl::shader_input* source_texture_input;
	
	gl::vertex_buffer* quad_vbo;
	gl::vertex_array* quad_vao;
};

} // namespace render

#endif // ANTKEEPER_RENDER_RESAMPLE_PASS_HPP

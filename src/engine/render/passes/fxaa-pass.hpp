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

#include <engine/render/pass.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture-2d.hpp>
#include <functional>

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
	 * Renders FXAA.
	 *
	 * @param ctx Render context.
	 * @param queue Render queue.
	 */
	void render(render::context& ctx) override;
	
	/**
	 * Sets the FXAA source texture.
	 *
	 * @param texture FXAA source texture.
	 */
	void set_source_texture(const gl::texture_2d* texture);

private:
	void rebuild_command_buffer();
	
	std::unique_ptr<gl::shader_program> shader;
	std::unique_ptr<gl::vertex_buffer> quad_vbo;
	std::unique_ptr<gl::vertex_array> quad_vao;
	
	const gl::texture_2d* source_texture{nullptr};
	
	std::vector<std::function<void()>> command_buffer;
};

} // namespace render

#endif // ANTKEEPER_RENDER_FXAA_PASS_HPP

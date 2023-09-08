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

#ifndef ANTKEEPER_GL_RASTERIZER_HPP
#define ANTKEEPER_GL_RASTERIZER_HPP

#include <engine/gl/drawing-mode.hpp>
#include <engine/gl/element-array-type.hpp>
#include <cstddef>
#include <memory>

namespace gl {

class framebuffer;
class vertex_array;
class shader_program;

/**
 * Interface to the OpenGL state and drawing functions.
 */
class rasterizer
{
public:
	/**
	 * Creates a rasterizer. Warning: This must be called after an OpenGL context has been created.
	 */
	rasterizer();
	
	/// Destroys a rasterizer.
	~rasterizer();

	/**
	 * This should be called when the window associated with the OpenGL context is resized, and will effectively changed the reported dimensions of the default framebuffer.
	 */
	void context_resized(int width, int height);
	
	/**
	 * Sets the active framebuffer.
	 *
	 * @param framebuffer Framebuffer to use.
	 */
	void use_framebuffer(const framebuffer& framebuffer);

	/**
	 * Sets the color to be used when the color buffer of a framebuffer is cleared.
	 *
	 * @param r Red color component.
	 * @param g Green color component.
	 * @param b Blue color component.
	 * @param a Alpha color component.
	 */
	void set_clear_color(float r, float g, float b, float a);
	
	/**
	 * Sets the depth value to be used when the depth buffer of a framebuffer is cleared.
	 *
	 * @param depth Depth value.
	 */
	void set_clear_depth(float depth);
	
	/**
	 * Sets the stencil value to be used when the stencil buffer of a framebuffer is cleared.
	 *
	 * @param s Stencil value.
	 */
	void set_clear_stencil(int s);
	
	/**
	 * Clears the buffers attached to a framebuffer.
	 *
	 * @param color Specifies whether the color buffer should be cleared.
	 * @param depth Specifies whether the depth buffer should be cleared.
	 * @param stencil Specifies whether the stencil buffer should be cleared.
	 */
	void clear_framebuffer(bool color, bool depth, bool stencil);
	
	/**
	 * Sets the active viewport.
	 *
	 * @param x X-coordinate of the viewport.
	 * @param y Y-coordinate of the viewport.
	 * @param width Width of the viewport.
	 * @param height Height of the viewport.
	 */
	void set_viewport(int x, int y, int width, int height);
	
	/**
	 * Binds a shader program.
	 *
	 * @param program Shader program to bind.
	 */
	void use_program(const shader_program& program);

	/**
	 *
	 */
	void draw_arrays(const vertex_array& vao, drawing_mode mode, std::size_t offset, std::size_t count);
	void draw_arrays(drawing_mode mode, std::size_t offset, std::size_t count);
	
	void draw_arrays_instanced(const vertex_array& vao, drawing_mode mode, std::size_t offset, std::size_t count, std::size_t instance_count);
	
	/**
	 *
	 */
	void draw_elements(const vertex_array& vao, drawing_mode mode, std::size_t offset, std::size_t count, element_array_type type);
	void draw_elements(drawing_mode mode, std::size_t offset, std::size_t count, element_array_type type);

	/**
	 * Returns the default framebuffer associated with the OpenGL context of a window.
	 */
	[[nodiscard]] inline const framebuffer& get_default_framebuffer() const noexcept
	{
		return *default_framebuffer;
	}

private:
	std::unique_ptr<framebuffer> default_framebuffer;
	std::unique_ptr<vertex_array> dummy_vao;
	const framebuffer* bound_framebuffer{nullptr};
	const vertex_array* bound_vao{nullptr};
	const shader_program* bound_shader_program{nullptr};
};

} // namespace gl

#endif // ANTKEEPER_GL_RASTERIZER_HPP

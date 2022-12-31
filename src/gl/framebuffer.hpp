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

#ifndef ANTKEEPER_GL_FRAMEBUFFER_HPP
#define ANTKEEPER_GL_FRAMEBUFFER_HPP

#include <array>

namespace gl {

class rasterizer;
class texture_2d;

enum class framebuffer_attachment_type
{
	color,
	depth,
	stencil
};

class framebuffer
{
public:
	/**
	 * Creates a framebuffer.
	 */
	framebuffer(int width, int height);
	
	/// Destroys a framebuffer.
	~framebuffer();
	
	/**
	 * Resizes the framebuffer. Note: This does not resize any attached textures.
	 *
	 * @param width New width of the framebuffer.
	 * @param height New height of the framebuffer.
	 */
	void resize(const std::array<int, 2>& dimensions);
	
	/**
	 * Attaches a color, depth, or stencil texture to the framebuffer.
	 *
	 * @param attachment_type Type of attachment.
	 */
	void attach(framebuffer_attachment_type attachment_type, texture_2d* texture);
	
	/// Returns the dimensions of the framebuffer, in pixels.
	const std::array<int, 2>& get_dimensions() const;
	
	const texture_2d* get_color_attachment() const;
	texture_2d* get_color_attachment();
	const texture_2d* get_depth_attachment() const;
	texture_2d* get_depth_attachment();
	const texture_2d* get_stencil_attachment() const;
	texture_2d* get_stencil_attachment();

private:
	friend class rasterizer;
	
	framebuffer();

	unsigned int gl_framebuffer_id;
	std::array<int, 2> dimensions;
	texture_2d* color_attachment;
	texture_2d* depth_attachment;
	texture_2d* stencil_attachment;
};

inline const std::array<int, 2>& framebuffer::get_dimensions() const
{
	return dimensions;
}

inline const texture_2d* framebuffer::get_color_attachment() const
{
	return color_attachment;
}

inline texture_2d* framebuffer::get_color_attachment()
{
	return color_attachment;
}

inline const texture_2d* framebuffer::get_depth_attachment() const
{
	return depth_attachment;
}

inline texture_2d* framebuffer::get_depth_attachment()
{
	return depth_attachment;
}

inline const texture_2d* framebuffer::get_stencil_attachment() const
{
	return stencil_attachment;
}

inline texture_2d* framebuffer::get_stencil_attachment()
{
	return stencil_attachment;
}

} // namespace gl

#endif // ANTKEEPER_GL_FRAMEBUFFER_HPP


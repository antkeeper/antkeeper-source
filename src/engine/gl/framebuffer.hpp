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
#include <cstdint>

namespace gl {

class rasterizer;
class texture;

enum class framebuffer_attachment_type: std::uint8_t
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
	framebuffer();
	
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
	 * @param texture Texture to attach.
	 * @param level Mip level of the texture to attach.
	 */
	void attach(framebuffer_attachment_type attachment_type, texture* texture, std::uint8_t level = 0);
	
	/// Returns the dimensions of the framebuffer, in pixels.
	[[nodiscard]] inline const std::array<int, 2>& get_dimensions() const noexcept
	{
		return m_dimensions;
	}
	
	/// Returns the attached color texture, if any.
	/// @{
	[[nodiscard]] inline const texture* get_color_attachment() const noexcept
	{
		return m_color_attachment;
	}
	[[nodiscard]] inline texture* get_color_attachment() noexcept
	{
		return m_color_attachment;
	}
	/// @}
	
	/// Returns the attached depth texture, if any.
	/// @{
	[[nodiscard]] inline const texture* get_depth_attachment() const noexcept
	{
		return m_depth_attachment;
	}
	[[nodiscard]] inline texture* get_depth_attachment() noexcept
	{
		return m_depth_attachment;
	}
	/// @}
	
	/// Returns the attached stencil texture, if any.
	/// @{
	[[nodiscard]] inline const texture* get_stencil_attachment() const noexcept
	{
		return m_stencil_attachment;
	}
	[[nodiscard]] inline texture* get_stencil_attachment() noexcept
	{
		return m_stencil_attachment;
	}
	/// @}

private:
	friend class rasterizer;
	
	unsigned int m_gl_framebuffer_id{0};
	std::array<int, 2> m_dimensions{0, 0};
	texture* m_color_attachment{nullptr};
	texture* m_depth_attachment{nullptr};
	texture* m_stencil_attachment{nullptr};
};

} // namespace gl

#endif // ANTKEEPER_GL_FRAMEBUFFER_HPP

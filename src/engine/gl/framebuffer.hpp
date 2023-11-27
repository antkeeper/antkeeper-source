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

#include <engine/gl/framebuffer-attachment.hpp>
#include <engine/gl/framebuffer-usage-bits.hpp>
#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace gl {

/**
 *
 */
class framebuffer
{
public:
	/**
	 * Constructs a framebuffer.
	 *
	 * @param attachments Framebuffer attachments.
	 * @param width Width of the framebuffer.
	 * @param height Height of the framebuffer.
	 */
	framebuffer(std::span<const framebuffer_attachment> attachments, std::uint32_t width, std::uint32_t height);
	
	/// Destroys a framebuffer.
	~framebuffer();
	
	/**
	 * Resizes the framebuffer.
	 *
	 * @param width New width of the framebuffer.
	 * @param height New height of the framebuffer.
	 *
	 * @warning Does not resize framebuffer attachments.
	 */
	void resize(std::uint32_t width, std::uint32_t height);
	
	/// Returns the framebuffer attachments.
	[[nodiscard]] inline constexpr const std::vector<framebuffer_attachment>& attachments() const noexcept
	{
		return m_attachments;
	}
	
	/// Returns the dimensions of the framebuffer.
	[[nodiscard]] inline constexpr const std::array<std::uint32_t, 2>& dimensions() const noexcept
	{
		return m_dimensions;
	}
	
	/// Returns the width of the framebuffer.
	[[nodiscard]] inline constexpr std::uint32_t width() const noexcept
	{
		return m_dimensions[0];
	}
	
	/// Returns the height of the framebuffer.
	[[nodiscard]] inline constexpr std::uint32_t height() const noexcept
	{
		return m_dimensions[1];
	}

private:
	friend class pipeline;
	
	std::vector<framebuffer_attachment> m_attachments;
	std::array<std::uint32_t, 2> m_dimensions{0, 0};
	unsigned int m_gl_named_framebuffer{0};
};

} // namespace gl

#endif // ANTKEEPER_GL_FRAMEBUFFER_HPP

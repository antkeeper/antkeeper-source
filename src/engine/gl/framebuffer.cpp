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

#include <engine/gl/framebuffer.hpp>
#include <engine/gl/texture.hpp>
#include <glad/glad.h>

namespace gl {

static constexpr GLenum attachment_lut[] =
{
	GL_COLOR_ATTACHMENT0,
	GL_DEPTH_ATTACHMENT,
	GL_STENCIL_ATTACHMENT
};

framebuffer::framebuffer(int width, int height):
	m_dimensions{width, height}
{
	glGenFramebuffers(1, &m_gl_framebuffer_id);
}

framebuffer::framebuffer():
	framebuffer(0, 0)
{}

framebuffer::~framebuffer()
{
	if (m_gl_framebuffer_id)
	{
		glDeleteFramebuffers(1, &m_gl_framebuffer_id);
	}
}

void framebuffer::resize(const std::array<int, 2>& dimensions)
{
	m_dimensions = dimensions;
}

void framebuffer::attach(framebuffer_attachment_type attachment_type, texture* texture, std::uint8_t level)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_gl_framebuffer_id);
	
	GLenum gl_attachment = attachment_lut[static_cast<std::size_t>(attachment_type)];
	glFramebufferTexture(GL_FRAMEBUFFER, gl_attachment, texture->m_gl_texture_id, level);
	
	if (attachment_type == framebuffer_attachment_type::color)
	{
		m_color_attachment = texture;
	}
	else if (attachment_type == framebuffer_attachment_type::depth)
	{
		m_depth_attachment = texture;
	}
	else if (attachment_type == framebuffer_attachment_type::stencil)
	{
		m_stencil_attachment = texture;
	}
	
	if (!m_color_attachment)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace gl

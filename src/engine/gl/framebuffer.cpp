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
#include <glad/gl.h>
#include <stdexcept>

namespace gl {

framebuffer::framebuffer(std::span<const framebuffer_attachment> attachments, std::uint32_t width, std::uint32_t height)
{
	m_dimensions = {width, height};
	m_attachments.assign(attachments.begin(), attachments.end());
	
	// Generate framebuffer
	glCreateFramebuffers(1, &m_gl_named_framebuffer);
	
	GLenum gl_color_attachment = GL_COLOR_ATTACHMENT0;
	std::vector<GLenum> gl_draw_buffers;
	
	// Attach textures to framebuffer
	for (const auto& attachment: m_attachments)
	{
		if (attachment.usage_mask & gl::color_attachment_bit)
		{
			glNamedFramebufferTexture
			(
				m_gl_named_framebuffer,
				gl_color_attachment,
				attachment.image_view->m_gl_texture_name,
				static_cast<GLuint>(attachment.level)
			);
			
			gl_draw_buffers.emplace_back(gl_color_attachment);
			++gl_color_attachment;
		}
		
		if (attachment.usage_mask & gl::depth_attachment_bit)
		{
			if (attachment.usage_mask & gl::stencil_attachment_bit)
			{
				glNamedFramebufferTexture
				(
					m_gl_named_framebuffer,
					GL_DEPTH_STENCIL_ATTACHMENT,
					attachment.image_view->m_gl_texture_name,
					static_cast<GLuint>(attachment.level)
				);
			}
			else
			{
				glNamedFramebufferTexture
				(
					m_gl_named_framebuffer,
					GL_DEPTH_ATTACHMENT,
					attachment.image_view->m_gl_texture_name,
					static_cast<GLuint>(attachment.level)
				);
			}
		}
		else if (attachment.usage_mask & gl::stencil_attachment_bit)
		{
			glNamedFramebufferTexture
			(
				m_gl_named_framebuffer,
				GL_STENCIL_ATTACHMENT,
				attachment.image_view->m_gl_texture_name,
				static_cast<GLuint>(attachment.level)
			);
		}
	}
	
	// Specify read and draw buffers
	if (!gl_draw_buffers.empty())
	{
		glNamedFramebufferReadBuffer(m_gl_named_framebuffer, GL_COLOR_ATTACHMENT0);
		glNamedFramebufferDrawBuffers
		(
			m_gl_named_framebuffer,
			static_cast<GLsizei>(gl_draw_buffers.size()),
			gl_draw_buffers.data()
		);
	}
	else
	{
		glNamedFramebufferReadBuffer(m_gl_named_framebuffer, GL_NONE);
		glNamedFramebufferDrawBuffer(m_gl_named_framebuffer, GL_NONE);
	}
	
	if (glCheckNamedFramebufferStatus(m_gl_named_framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		throw std::runtime_error("OpenGL framebuffer incomplete.");
	}
}

framebuffer::~framebuffer()
{
	if (m_gl_named_framebuffer)
	{
		glDeleteFramebuffers(1, &m_gl_named_framebuffer);
	}
}

void framebuffer::resize(std::uint32_t width, std::uint32_t height)
{
	m_dimensions = {width, height};
}

} // namespace gl

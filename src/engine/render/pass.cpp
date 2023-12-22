// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/pass.hpp>

namespace render {

pass::pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer):
	m_pipeline(pipeline),
	m_framebuffer(framebuffer),
	m_enabled(true)
{}

void pass::set_enabled(bool enabled)
{
	m_enabled = enabled;
}

void pass::set_framebuffer(const gl::framebuffer* framebuffer)
{
	m_framebuffer = framebuffer;
}

void pass::clear()
{
	m_pipeline->clear_attachments(m_clear_mask, m_clear_value);
}

} // namespace render

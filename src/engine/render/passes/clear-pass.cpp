// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/passes/clear-pass.hpp>

namespace render {

clear_pass::clear_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer):
	pass(pipeline, framebuffer)
{}

void clear_pass::render([[maybe_unused]] render::context& ctx)
{
	if (m_clear_mask)
	{
		m_pipeline->bind_framebuffer(m_framebuffer);
		clear();
	}
}

} // namespace render

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.render.passes.clear_pass;
import engine.gl.pipeline;

namespace engine::render
{
	clear_pass::clear_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer):
		pass(pipeline, framebuffer)
	{}

	void clear_pass::render(render::context&)
	{
		if (m_clear_mask)
		{
			m_pipeline->bind_framebuffer(m_framebuffer);
			clear();
		}
	}
}

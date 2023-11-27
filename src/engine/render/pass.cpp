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

#include <engine/render/pass.hpp>

namespace render {

pass::pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer):
	m_pipeline(pipeline),
	m_framebuffer(framebuffer),
	m_enabled(true)
{}

pass::~pass()
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

/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "renderer/passes/clear-pass.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include <glad/glad.h>

clear_pass::clear_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer):
	render_pass(rasterizer, framebuffer),
	clear_color_buffer(false),
	clear_depth_buffer(false),
	clear_stencil_buffer(false),
	clear_color({0.0f, 0.0f, 0.0f, 0.0f}),
	clear_depth(1.0f),
	clear_stencil(0)
{}

clear_pass::~clear_pass()
{}

void clear_pass::render(render_context* context) const
{
	if (clear_color_buffer)
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if (clear_depth_buffer)
		glDepthMask(GL_TRUE);
	if (clear_stencil_buffer)
		glStencilMask(0xFF);
	
	rasterizer->use_framebuffer(*framebuffer);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	rasterizer->set_clear_color(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	rasterizer->set_clear_depth(clear_depth);
	rasterizer->set_clear_stencil(clear_stencil);
	rasterizer->clear_framebuffer(clear_color_buffer, clear_depth_buffer, clear_stencil_buffer);
}

void clear_pass::set_cleared_buffers(bool color, bool depth, bool stencil)
{
	clear_color_buffer = color;
	clear_depth_buffer = depth;
	clear_stencil_buffer = stencil;
}

void clear_pass::set_clear_color(const float4& color)
{
	clear_color = color;
}

void clear_pass::set_clear_depth(float depth)
{
	clear_depth = depth;
}

void clear_pass::set_clear_stencil(int stencil)
{
	clear_stencil = stencil;
}

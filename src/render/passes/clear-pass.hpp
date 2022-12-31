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

#ifndef ANTKEEPER_RENDER_CLEAR_PASS_HPP
#define ANTKEEPER_RENDER_CLEAR_PASS_HPP

#include "render/pass.hpp"
#include "utility/fundamental-types.hpp"

namespace render {

/**
 * Clears the color, depth, or stencil buffer of a render target.
 */
class clear_pass: public pass
{
public:
	clear_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer);
	virtual ~clear_pass();
	virtual void render(const render::context& ctx, render::queue& queue) const final;
	
	/**
	 * Sets the buffers to be cleared.
	 *
	 * @param color Clear the color buffer.
	 * @param depth Clear the depth buffer.
	 * @param stencil Clear the stencil buffer.
	 */
	void set_cleared_buffers(bool color, bool depth, bool stencil);
	
	/**
	 * Sets color buffer clear color.
	 *
	 * @param color Clear color.
	 */
	void set_clear_color(const float4& color);
	
	/**
	 * Sets the depth buffer clear value.
	 *
	 * @param depth Clear value.
	 */
	void set_clear_depth(float depth);
	
	/**
	 * Sets the stencil buffer clear value.
	 *
	 * @param stencil Clear value.
	 */
	void set_clear_stencil(int stencil);

private:
	bool clear_color_buffer;
	bool clear_depth_buffer;
	bool clear_stencil_buffer;
	float4 clear_color;
	float clear_depth;
	int clear_stencil;
};

} // namespace render

#endif // ANTKEEPER_RENDER_CLEAR_PASS_HPP


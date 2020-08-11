/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_CLEAR_PASS_HPP
#define ANTKEEPER_CLEAR_PASS_HPP

#include "renderer/render-pass.hpp"
#include "utility/fundamental-types.hpp"

/**
 * Clears the color, depth, or stencil buffer of a render target.
 */
class clear_pass: public render_pass
{
public:
	clear_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer);
	virtual ~clear_pass();
	virtual void render(render_context* context) const final;
	
	void set_cleared_buffers(bool color, bool depth, bool stencil);
	
	void set_clear_color(const float4& color);
	void set_clear_depth(float depth);
	void set_clear_stencil(int stencil);

private:
	bool clear_color_buffer;
	bool clear_depth_buffer;
	bool clear_stencil_buffer;
	float4 clear_color;
	float clear_depth;
	int clear_stencil;
};

#endif // ANTKEEPER_CLEAR_PASS_HPP


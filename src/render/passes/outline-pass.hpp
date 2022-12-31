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

#ifndef ANTKEEPER_RENDER_OUTLINE_PASS_HPP
#define ANTKEEPER_RENDER_OUTLINE_PASS_HPP

#include "render/pass.hpp"
#include "utility/fundamental-types.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"

class resource_manager;

namespace render {

/**
 *
 */
class outline_pass: public pass
{
public:
	outline_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~outline_pass();
	virtual void render(const render::context& ctx, render::queue& queue) const final;
	
	void set_outline_width(float width);
	void set_outline_color(const float4& color);

private:
	gl::shader_program* fill_shader;
	const gl::shader_input* fill_model_view_projection_input;
	
	gl::shader_program* stroke_shader;
	const gl::shader_input* stroke_model_view_projection_input;
	const gl::shader_input* stroke_width_input;
	const gl::shader_input* stroke_color_input;
	
	float outline_width;
	float4 outline_color;
};

} // namespace render

#endif // ANTKEEPER_RENDER_OUTLINE_PASS_HPP

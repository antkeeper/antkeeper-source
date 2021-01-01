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

#ifndef ANTKEEPER_OUTLINE_PASS_HPP
#define ANTKEEPER_OUTLINE_PASS_HPP

#include "renderer/render-pass.hpp"
#include "utility/fundamental-types.hpp"

class shader_program;
class shader_input;
class resource_manager;

/**
 *
 */
class outline_pass: public render_pass
{
public:
	outline_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~outline_pass();
	virtual void render(render_context* context) const final;
	
	void set_outline_width(float width);
	void set_outline_color(const float4& color);

private:
	shader_program* fill_shader;
	const shader_input* fill_model_view_projection_input;
	
	shader_program* stroke_shader;
	const shader_input* stroke_model_view_projection_input;
	const shader_input* stroke_width_input;
	const shader_input* stroke_color_input;
	
	float outline_width;
	float4 outline_color;
};

#endif // ANTKEEPER_OUTLINE_PASS_HPP


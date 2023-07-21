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

#include <engine/render/pass.hpp>
#include <engine/math/vector.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <memory>

class resource_manager;

namespace render {

/**
 *
 */
class outline_pass: public pass
{
public:
	outline_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~outline_pass() = default;
	void render(render::context& ctx) override;
	
	void set_outline_width(float width);
	void set_outline_color(const math::fvec4& color);

private:
	std::unique_ptr<gl::shader_program> fill_shader;
	const gl::shader_variable* fill_model_view_projection_var;
	
	std::unique_ptr<gl::shader_program> stroke_shader;
	const gl::shader_variable* stroke_model_view_projection_var;
	const gl::shader_variable* stroke_width_var;
	const gl::shader_variable* stroke_color_var;
	
	float outline_width;
	math::fvec4 outline_color;
};

} // namespace render

#endif // ANTKEEPER_RENDER_OUTLINE_PASS_HPP

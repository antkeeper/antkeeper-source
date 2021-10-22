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

#ifndef ANTKEEPER_RENDER_UI_PASS_HPP
#define ANTKEEPER_RENDER_UI_PASS_HPP

#include "render/pass.hpp"
#include "render/material.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/texture-2d.hpp"
#include <unordered_map>

class resource_manager;

namespace render {

/**
 *
 */
class ui_pass: public pass
{
public:
	ui_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~ui_pass();
	virtual void render(const render::context& ctx, render::queue& queue) const final;

private:
	/**
	 * Sets of known shader input parameters. Each time a new shader is encountered, a parameter set will be created and its inputs connected to the shader program. A null input indiciates that the shader doesn't have that parameter.
	 */
	struct parameter_set
	{
		const gl::shader_input* time;
		const gl::shader_input* model_view_projection;
	};

	const parameter_set* load_parameter_set(const gl::shader_program* program) const;

	mutable std::unordered_map<const gl::shader_program*, parameter_set*> parameter_sets;
};

} // namespace render

#endif // ANTKEEPER_RENDER_UI_PASS_HPP

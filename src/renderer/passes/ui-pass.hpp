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

#ifndef ANTKEEPER_UI_PASS_HPP
#define ANTKEEPER_UI_PASS_HPP

#include "renderer/render-pass.hpp"
#include "renderer/material.hpp"
#include <unordered_map>

class shader_program;
class shader_input;
class resource_manager;
class texture_2d;

/**
 *
 */
class ui_pass: public render_pass
{
public:
	ui_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~ui_pass();
	virtual void render(render_context* context) const final;

	void set_time(float time);

private:
	/**
	 * Sets of known shader input parameters. Each time a new shader is encountered, a parameter set will be created and its inputs connected to the shader program. A null input indiciates that the shader doesn't have that parameter.
	 */
	struct parameter_set
	{
		const shader_input* time;
		const shader_input* model_view_projection;
	};

	const parameter_set* load_parameter_set(const shader_program* program) const;

	mutable std::unordered_map<const shader_program*, parameter_set*> parameter_sets;
	float time;
};

#endif // ANTKEEPER_UI_PASS_HPP


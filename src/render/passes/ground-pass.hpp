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

#ifndef ANTKEEPER_RENDER_GROUND_PASS_HPP
#define ANTKEEPER_RENDER_GROUND_PASS_HPP

#include "render/pass.hpp"
#include "utility/fundamental-types.hpp"
#include "animation/tween.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/drawing-mode.hpp"

class resource_manager;

namespace render {

class material;
class model;

/**
 *
 */
class ground_pass: public pass
{
public:
	ground_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager);
	virtual ~ground_pass();
	virtual void render(const render::context& ctx, render::queue& queue) const final;
	
	void set_ground_model(const model* model);

private:	
	gl::shader_program* shader_program;
	const gl::shader_input* model_view_projection_input;
	const gl::shader_input* view_projection_input;
	const gl::shader_input* camera_position_input;
	const gl::shader_input* directional_light_colors_input;
	const gl::shader_input* directional_light_directions_input;
	const gl::shader_input* ambient_light_colors_input;
	
	const model* ground_model;
	const material* ground_material;
	const gl::vertex_array* ground_model_vao;
	gl::drawing_mode ground_model_drawing_mode;
	std::size_t ground_model_start_index;
	std::size_t ground_model_index_count;
};

} // namespace render

#endif // ANTKEEPER_RENDER_GROUND_PASS_HPP

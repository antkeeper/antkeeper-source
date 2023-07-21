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

#ifndef ANTKEEPER_RENDER_GROUND_PASS_HPP
#define ANTKEEPER_RENDER_GROUND_PASS_HPP

#include <engine/render/pass.hpp>
#include <engine/math/vector.hpp>
#include <engine/animation/tween.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/drawing-mode.hpp>

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
	void render(render::context& ctx) override;
	
	void set_ground_model(std::shared_ptr<render::model> model);

private:	
	std::shared_ptr<gl::shader_program> shader_program;
	const gl::shader_variable* model_view_projection_var;
	const gl::shader_variable* view_projection_var;
	const gl::shader_variable* camera_position_var;
	const gl::shader_variable* directional_light_colors_var;
	const gl::shader_variable* directional_light_directions_var;
	
	std::shared_ptr<model> ground_model;
	const material* ground_material;
	const gl::vertex_array* ground_model_vao;
	gl::drawing_mode ground_model_drawing_mode;
	std::size_t ground_model_start_index;
	std::size_t ground_model_index_count;
};

} // namespace render

#endif // ANTKEEPER_RENDER_GROUND_PASS_HPP

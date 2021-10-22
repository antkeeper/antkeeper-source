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

#ifndef ANTKEEPER_RENDER_SIMPLE_PASS_HPP
#define ANTKEEPER_RENDER_SIMPLE_PASS_HPP

#include "render/pass.hpp"
#include "utility/fundamental-types.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"

namespace render {

class material;
template <class T>
class material_property;

/**
 * Simple render passes are associated with a single shader and a single material.
 */
class simple_pass: public pass
{
public:
	simple_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, gl::shader_program* shader_program);
	virtual ~simple_pass();
	virtual void render(const render::context& ctx, render::queue& queue) const final;
	
	const render::material* get_material() const;
	render::material* get_material();

private:
	gl::shader_program* shader_program;
	material* material;
	material_property<float>* time_property;
	material_property<float2>* resolution_property;
	
	gl::vertex_buffer* quad_vbo;
	gl::vertex_array* quad_vao;
};

inline const render::material* simple_pass::get_material() const
{
	return material;
}

inline render::material* simple_pass::get_material()
{
	return material;
}

} // namespace render

#endif // ANTKEEPER_RENDER_SIMPLE_PASS_HPP

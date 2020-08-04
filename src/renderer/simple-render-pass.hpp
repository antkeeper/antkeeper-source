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

#ifndef ANTKEEPER_SIMPLE_RENDER_PASS_HPP
#define ANTKEEPER_SIMPLE_RENDER_PASS_HPP

#include "renderer/render-pass.hpp"
#include "animation/tween.hpp"
#include <vmq/vmq.hpp>

using namespace vmq::types;

class shader_program;
class shader_input;
class vertex_buffer;
class vertex_array;
class material;
template <class T>
class material_property;

/**
 * Simple render passes are associated with a single shader and a single material.
 */
class simple_render_pass: public render_pass
{
public:
	simple_render_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, ::shader_program* shader_program);
	virtual ~simple_render_pass();
	virtual void render(render_context* context) const final;
	
	void set_time_tween(const tween<double>* time);
	
	const ::material* get_material() const;
	::material* get_material();

private:
	shader_program* shader_program;
	material* material;
	material_property<float>* time_property;
	material_property<float2>* resolution_property;
	
	const tween<double>* time_tween;
	
	vertex_buffer* quad_vbo;
	vertex_array* quad_vao;
};

inline const ::material* simple_render_pass::get_material() const
{
	return material;
}

inline ::material* simple_render_pass::get_material()
{
	return material;
}

#endif // ANTKEEPER_SIMPLE_RENDER_PASS_HPP

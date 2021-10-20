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

#include "renderer/simple-render-pass.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "renderer/vertex-attribute.hpp"
#include "renderer/context.hpp"
#include "renderer/material.hpp"
#include "renderer/material-property.hpp"
#include "math/math.hpp"
#include <glad/glad.h>

simple_render_pass::simple_render_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, gl::shader_program* shader_program):
	render_pass(rasterizer, framebuffer),
	shader_program(shader_program)
{
	// Create material
	material = new ::material(shader_program);
	
	// Add standard properties to material
	time_property = material->add_property<float>("time");
	resolution_property = material->add_property<float2>("resolution");
	
	const float vertex_data[] =
	{
		-1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};

	std::size_t vertex_size = 3;
	std::size_t vertex_stride = sizeof(float) * vertex_size;
	std::size_t vertex_count = 6;

	quad_vbo = new gl::vertex_buffer(sizeof(float) * vertex_size * vertex_count, vertex_data);
	quad_vao = new gl::vertex_array();
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = quad_vbo;
	position_attribute.offset = 0;
	position_attribute.stride = vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	
	// Bind vertex attributes to VAO
	quad_vao->bind(render::vertex_attribute::position, position_attribute);
}

simple_render_pass::~simple_render_pass()
{
	delete material;
	delete quad_vao;
	delete quad_vbo;
}

void simple_render_pass::render(const render::context& ctx, render::queue& queue) const
{
	// Bind framebuffer
	rasterizer->use_framebuffer(*framebuffer);
	
	// Setup graphics context
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Setup viewport
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	// Change shader program
	rasterizer->use_program(*shader_program);
	
	// Update material properties
	time_property->set_value(ctx.t);
	resolution_property->set_value({static_cast<float>(std::get<0>(viewport)), static_cast<float>(std::get<1>(viewport))});
	
	// Upload material properties
	material->upload(ctx.alpha);

	// Draw quad
	rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
}

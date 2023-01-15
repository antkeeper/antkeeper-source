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

#include "render/passes/resample-pass.hpp"
#include "resources/resource-manager.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/texture-2d.hpp"
#include "render/vertex-attribute.hpp"
#include "render/context.hpp"
#include <glad/glad.h>

namespace render {

resample_pass::resample_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	source_texture(nullptr)
{
	// Load resample shader
	shader = resource_manager->load<gl::shader_program>("resample.glsl");
	source_texture_input = shader->get_input("source_texture");

	const float vertex_data[] =
	{
		-1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f,  1.0f,
		 1.0f,  1.0f,
		-1.0f, -1.0f,
		 1.0f, -1.0f
	};
	
	std::size_t vertex_size = 2;
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
	position_attribute.components = 2;
	
	// Bind vertex attributes to VAO
	quad_vao->bind(render::vertex_attribute::position, position_attribute);
}

resample_pass::~resample_pass()
{
	delete quad_vao;
	delete quad_vbo;
}

void resample_pass::render(const render::context& ctx, render::queue& queue) const
{
	if (!source_texture)
		return;
	
	// Set rasterizer state
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);
	
	// Render FXAA
	rasterizer->use_framebuffer(*framebuffer);
	rasterizer->set_viewport(0, 0, framebuffer->get_dimensions()[0], framebuffer->get_dimensions()[1]);
	rasterizer->use_program(*shader);
	source_texture_input->upload(source_texture);
	rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
}

void resample_pass::set_source_texture(const gl::texture_2d* texture)
{
	source_texture = texture;
}

} // namespace render

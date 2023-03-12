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

#include <engine/render/passes/fxaa-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-attribute.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/render/context.hpp>
#include <engine/debug/log.hpp>
#include <glad/glad.h>

namespace render {

fxaa_pass::fxaa_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer)
{
	// Load FXAA shader template
	auto shader_template = resource_manager->load<gl::shader_template>("fxaa.glsl");
	
	// Build FXAA shader program
	shader = shader_template->build();

	const float2 vertex_positions[] =
	{
		{-1.0f,  1.0f},
		{-1.0f, -1.0f},
		{ 1.0f,  1.0f},
		{ 1.0f,  1.0f},
		{-1.0f, -1.0f},
		{ 1.0f, -1.0f}
	};
	
	const auto vertex_data = std::as_bytes(std::span{vertex_positions});
	std::size_t vertex_size = 2;
	std::size_t vertex_stride = sizeof(float) * vertex_size;
	
	quad_vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data.size(), vertex_data);
	quad_vao = std::make_unique<gl::vertex_array>();
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = quad_vbo.get();
	position_attribute.offset = 0;
	position_attribute.stride = vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 2;
	
	// Bind vertex attributes to VAO
	quad_vao->bind(render::vertex_attribute::position, position_attribute);
}

void fxaa_pass::render(render::context& ctx)
{
	for (const auto& command: command_buffer)
	{
		command();
	}
}

void fxaa_pass::set_source_texture(const gl::texture_2d* texture)
{
	source_texture = texture;
	rebuild_command_buffer();
}

void fxaa_pass::rebuild_command_buffer()
{
	command_buffer.clear();
	
	if (!source_texture || !shader)
	{
		return;
	}
	
	// Setup FXAA state
	command_buffer.emplace_back
	(
		[&]()
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDisable(GL_BLEND);
			
			// Render FXAA
			rasterizer->use_framebuffer(*framebuffer);
			rasterizer->set_viewport(0, 0, framebuffer->get_dimensions()[0], framebuffer->get_dimensions()[1]);
			rasterizer->use_program(*shader);
		}
	);
	
	// Update shader variables
	if (auto source_texture_var = shader->variable("source_texture"))
	{
		command_buffer.emplace_back([&, source_texture_var](){source_texture_var->update(*source_texture);});
	}
	if (auto texel_size_var = shader->variable("texel_size"))
	{
		command_buffer.emplace_back
		(
			[&, texel_size_var]()
			{
				const float2 texel_size = 1.0f / float2{static_cast<float>(source_texture->get_width()), static_cast<float>(source_texture->get_height())};
				texel_size_var->update(texel_size);
			}
		);
	}
	
	// Draw quad
	command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
		}
	);
}

} // namespace render

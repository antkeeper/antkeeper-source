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

#include <engine/render/passes/resample-pass.hpp>
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
#include <glad/glad.h>

namespace render {

resample_pass::resample_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	source_texture{nullptr}
{
	// Load resample shader template
	auto shader_template = resource_manager->load<gl::shader_template>("resample.glsl");
	
	// Build resample shader program
	shader = shader_template->build();
	if (!shader->linked())
	{
		debug::log::error("Failed to build resample shader program: {}", shader->info());
		debug::log::warning("{}", shader_template->configure(gl::shader_stage::vertex));
	}
}

void resample_pass::render(render::context& ctx)
{
	for (const auto& command: command_buffer)
	{
		command();
	}
}

void resample_pass::set_source_texture(const gl::texture_2d* texture)
{
	source_texture = texture;
	
	rebuild_command_buffer();
}

void resample_pass::rebuild_command_buffer()
{
	command_buffer.clear();
	
	if (!source_texture || !shader)
	{
		return;
	}
	
	// Setup resample state
	command_buffer.emplace_back
	(
		[&]()
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDisable(GL_BLEND);
			
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
	
	// Draw quad
	command_buffer.emplace_back
	(
		[&]()
		{
			rasterizer->draw_arrays(gl::drawing_mode::triangles, 0, 3);
		}
	);
}

} // namespace render

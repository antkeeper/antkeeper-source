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

#include "render/passes/outline-pass.hpp"
#include "resources/resource-manager.hpp"
#include "gl/rasterizer.hpp"
#include "gl/framebuffer.hpp"
#include "gl/shader-program.hpp"
#include "gl/shader-input.hpp"
#include "gl/vertex-buffer.hpp"
#include "gl/vertex-array.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/drawing-mode.hpp"
#include "render/vertex-attribute.hpp"
#include "render/context.hpp"
#include "render/material.hpp"
#include "render/material-flags.hpp"
#include "scene/camera.hpp"
#include <cmath>
#include <glad/glad.h>

namespace render {

outline_pass::outline_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	fill_shader(nullptr),
	stroke_shader(nullptr)
{
	// Load fill shader
	fill_shader = resource_manager->load<gl::shader_program>("outline-fill-unskinned.glsl");
	fill_model_view_projection_input = fill_shader->get_input("model_view_projection");
	
	// Load stroke shader
	stroke_shader = resource_manager->load<gl::shader_program>("outline-stroke-unskinned.glsl");
	stroke_model_view_projection_input = stroke_shader->get_input("model_view_projection");
	stroke_width_input = stroke_shader->get_input("width");
	stroke_color_input = stroke_shader->get_input("color");
}

outline_pass::~outline_pass()
{}

void outline_pass::render(const render::context& ctx, render::queue& queue) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	// Determine viewport based on framebuffer resolution
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	// Get camera matrices
	float4x4 view = ctx.camera->get_view_tween().interpolate(ctx.alpha);
	float4x4 view_projection = ctx.camera->get_view_projection_tween().interpolate(ctx.alpha);
	
	float4x4 model_view_projection;
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	
	// Render fill
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  
		glStencilFunc(GL_ALWAYS, 2, 0xFF);
		glStencilMask(0xFF);
		glDisable(GL_BLEND);
		
		// Setup fill shader
		rasterizer->use_program(*fill_shader);
		
		// Render fills
		for (const render::operation& operation: queue)
		{
			const render::material* material = operation.material;
			if (!material || !(material->get_flags() & MATERIAL_FLAG_OUTLINE))
				continue;
			
			model_view_projection = view_projection * operation.transform;
			fill_model_view_projection_input->upload(model_view_projection);
			
			rasterizer->draw_arrays(*operation.vertex_array, operation.drawing_mode, operation.start_index, operation.index_count);
		}
	}
	
	// Render stroke
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		if (outline_color[3] < 1.0f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glDisable(GL_BLEND);
		}

		glStencilFunc(GL_NOTEQUAL, 2, 0xFF);
		glStencilMask(0x00);
		
		// Setup stroke shader
		rasterizer->use_program(*stroke_shader);
		stroke_width_input->upload(outline_width);
		stroke_color_input->upload(outline_color);
		
		// Render strokes
		for (const render::operation& operation: queue)
		{
			const render::material* material = operation.material;
			if (!material || !(material->get_flags() & MATERIAL_FLAG_OUTLINE))
				continue;
			
			model_view_projection = view_projection * operation.transform;
			stroke_model_view_projection_input->upload(model_view_projection);
			
			rasterizer->draw_arrays(*operation.vertex_array, operation.drawing_mode, operation.start_index, operation.index_count);
		}
	}
	
	glDisable(GL_STENCIL_TEST);
}

void outline_pass::set_outline_width(float width)
{
	outline_width = width;
}

void outline_pass::set_outline_color(const float4& color)
{
	outline_color = color;
}

} // namespace render

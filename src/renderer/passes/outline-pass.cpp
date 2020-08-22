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

#include "renderer/passes/outline-pass.hpp"
#include "resources/resource-manager.hpp"
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
#include "rasterizer/shader-program.hpp"
#include "rasterizer/shader-input.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "rasterizer/vertex-array.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "rasterizer/drawing-mode.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/render-context.hpp"
#include "renderer/material.hpp"
#include "renderer/material-flags.hpp"
#include "scene/camera.hpp"
#include "math/math.hpp"
#include <cmath>
#include <glad/glad.h>

outline_pass::outline_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	outline_shader(nullptr)
{
	// Load outline shader
	outline_shader = resource_manager->load<shader_program>("outline-unskinned.glsl");
	model_view_projection_input = outline_shader->get_input("model_view_projection");
	outline_width_input = outline_shader->get_input("outline_width");
	outline_color_input = outline_shader->get_input("outline_color");
}

outline_pass::~outline_pass()
{}

void outline_pass::render(render_context* context) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	if (outline_color.w < 1.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
	
	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);

	// Determine viewport based on framebuffer resolution
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float4x4 view = context->camera->get_view_tween().interpolate(context->alpha);
	float4x4 view_projection = context->camera->get_view_projection_tween().interpolate(context->alpha);
	float4x4 model_view_projection;
	
	// Perform iterative blur subpass
	rasterizer->use_program(*outline_shader);
	
	outline_width_input->upload(outline_width);
	outline_color_input->upload(outline_color);
	
	// Render outlines
	for (const render_operation& operation: context->operations)
	{
		const ::material* material = operation.material;
		if (!material || !(material->get_flags() & MATERIAL_FLAG_OUTLINE))
			continue;
		
		model_view_projection = view_projection * operation.transform;
		model_view_projection_input->upload(model_view_projection);
		
		rasterizer->draw_arrays(*operation.vertex_array, operation.drawing_mode, operation.start_index, operation.index_count);
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

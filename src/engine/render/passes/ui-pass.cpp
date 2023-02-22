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

#include <engine/render/passes/ui-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/rasterizer.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-input.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-attribute.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/render/material-flags.hpp>
#include <engine/render/context.hpp>
#include <engine/scene/camera.hpp>
#include <engine/scene/collection.hpp>
#include <engine/scene/ambient-light.hpp>
#include <engine/scene/directional-light.hpp>
#include <engine/scene/billboard.hpp>
#include <cmath>
#include <glad/glad.h>

namespace render {

ui_pass::ui_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer)
{}

ui_pass::~ui_pass()
{}

void ui_pass::render(const render::context& ctx, render::queue& queue) const
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));

	float4x4 view = ctx.camera->get_view_tween().interpolate(ctx.alpha);
	float4x4 projection = ctx.camera->get_projection_tween().interpolate(ctx.alpha);
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection;
	
	// Collect billboards
	std::list<scene::object_base*> billboards = *ctx.collection->get_objects(scene::billboard::object_type_id);
	
	// Sort billboards
	
	// Rebuild vertex buffer
}

const ui_pass::parameter_set* ui_pass::load_parameter_set(const gl::shader_program* program) const
{
	// Allocate a new parameter set
	parameter_set* parameters = new parameter_set();

	// Connect inputs
	parameters->time = program->get_input("time");
	parameters->model_view_projection = program->get_input("model_view_projection");

	// Add parameter set to map of parameter sets
	parameter_sets[program] = parameters;

	return parameters;
}

} // namespace render

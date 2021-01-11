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

#include "renderer/passes/ui-pass.hpp"
#include "resources/resource-manager.hpp"
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
#include "rasterizer/shader.hpp"
#include "rasterizer/shader-type.hpp"
#include "rasterizer/shader-program.hpp"
#include "rasterizer/shader-input.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "rasterizer/vertex-array.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "rasterizer/drawing-mode.hpp"
#include "rasterizer/texture-2d.hpp"
#include "rasterizer/texture-wrapping.hpp"
#include "rasterizer/texture-filter.hpp"
#include "renderer/vertex-attributes.hpp"
#include "renderer/material-flags.hpp"
#include "renderer/render-context.hpp"
#include "scene/camera.hpp"
#include "scene/collection.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/billboard.hpp"
#include "math/math.hpp"
#include <cmath>
#include <glad/glad.h>

ui_pass::ui_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	time(0.0f)
{}

ui_pass::~ui_pass()
{}

void ui_pass::render(render_context* context) const
{
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));

	float4x4 view = context->camera->get_view_tween().interpolate(context->alpha);
	float4x4 projection = context->camera->get_projection_tween().interpolate(context->alpha);
	float4x4 view_projection = projection * view;
	float4x4 model_view_projection;
	
	// Collect billboards
	std::list<scene::object_base*> billboards = *context->collection->get_objects(scene::billboard::object_type_id);
	
	// Sort billboards
	
	// Rebuild vertex buffer
}

void ui_pass::set_time(float time)
{
	this->time = time;
}

const ui_pass::parameter_set* ui_pass::load_parameter_set(const shader_program* program) const
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

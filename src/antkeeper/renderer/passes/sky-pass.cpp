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

#include "renderer/passes/sky-pass.hpp"
#include "resources/resource-manager.hpp"
#include "rasterizer/rasterizer.hpp"
#include "rasterizer/framebuffer.hpp"
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
#include "renderer/render-context.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "scene/ambient-light.hpp"
#include "scene/directional-light.hpp"
#include "scene/scene.hpp"
#include <vmq/vmq.hpp>
#include <cmath>
#include <glad/glad.h>

using namespace vmq;

sky_pass::sky_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer)
{
	shader_program = resource_manager->load<::shader_program>("sky.glsl");
	matrix_input = shader_program->get_input("matrix");
	sun_direction_input = shader_program->get_input("sun_direction");
	sun_angular_radius_input = shader_program->get_input("sun_angular_radius");
	sky_gradient_input = shader_program->get_input("sky_gradient");
	bayer_matrix_input = shader_program->get_input("bayer_matrix");

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

	quad_vbo = new vertex_buffer(sizeof(float) * vertex_size * vertex_count, vertex_data);
	quad_vao = new vertex_array();
	quad_vao->bind_attribute(VERTEX_POSITION_LOCATION, *quad_vbo, 3, vertex_attribute_type::float_32, vertex_stride, 0);

	sky_gradient = resource_manager->load<texture_2d>("grassland-sky-gradient.png");
	sky_gradient->set_wrapping(texture_wrapping::clamp, texture_wrapping::clamp);
	sky_gradient->set_filters(texture_min_filter::linear, texture_mag_filter::linear);
	
	// Generate Bayer matrix texture
	static const char bayer_matrix_data[] =
	{
		 0, 32,  8, 40,  2, 34, 10, 42, 
		48, 16, 56, 24, 50, 18, 58, 26,
		12, 44,  4, 36, 14, 46,  6, 38,
		60, 28, 52, 20, 62, 30, 54, 22,
		 3, 35, 11, 43,  1, 33,  9, 41,
		51, 19, 59, 27, 49, 17, 57, 25,
		15, 47,  7, 39, 13, 45,  5, 37,
		63, 31, 55, 23, 61, 29, 53, 21
	};
	bayer_matrix = new texture_2d(8, 8, pixel_type::int_8, pixel_format::r, bayer_matrix_data);
	bayer_matrix->set_wrapping(texture_wrapping::repeat, texture_wrapping::repeat);
	bayer_matrix->set_filters(texture_min_filter::nearest, texture_mag_filter::nearest);
	bayer_matrix->set_max_anisotropy(0.0f);
}

sky_pass::~sky_pass()
{
	delete bayer_matrix;
	delete quad_vao;
	delete quad_vbo;
}

void sky_pass::render(render_context* context) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));

	float3 sun_direction = {0, 0, -1};
	float sun_angular_radius = 3.0f * vmq::pi<float> / 180.0f;
	
	// Find sun direction
	const std::list<scene_object_base*>* lights = context->scene->get_objects(light::object_type_id);
	for (const scene_object_base* object: *lights)
	{
		const ::light* light = static_cast<const ::light*>(object);
		
		if (light->get_light_type() == light_type::directional)
		{
			sun_direction = static_cast<const directional_light*>(light)->get_direction_tween().interpolate(context->alpha);
			break;
		}
	}

	// Calculate matrix
	float4x4 model_view = vmq::resize<4, 4>(vmq::resize<3, 3>(context->camera->get_view_tween().interpolate(context->alpha)));
	float4x4 inverse_projection = vmq::inverse(context->camera->get_projection_tween().interpolate(context->alpha));
	float4x4 matrix = vmq::inverse(model_view) * inverse_projection;

	// Change shader program
	rasterizer->use_program(*shader_program);

	// Upload shader parameters
	matrix_input->upload(matrix);
	sun_direction_input->upload(sun_direction);
	sun_angular_radius_input->upload(sun_angular_radius);
	sky_gradient_input->upload(sky_gradient);
	bayer_matrix_input->upload(bayer_matrix);

	// Draw quad
	rasterizer->draw_arrays(*quad_vao, drawing_mode::triangles, 0, 6);
}


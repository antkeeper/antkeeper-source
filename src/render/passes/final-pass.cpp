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

#include "render/passes/final-pass.hpp"
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
#include "gl/texture-wrapping.hpp"
#include "gl/texture-filter.hpp"
#include "render/vertex-attribute.hpp"
#include "render/context.hpp"
#include <cmath>
#include <glad/glad.h>

namespace render {

final_pass::final_pass(gl::rasterizer* rasterizer, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(rasterizer, framebuffer),
	color_texture(nullptr),
	bloom_texture(nullptr),
	bloom_weight(0.04f),
	blue_noise_texture(nullptr),
	blue_noise_scale(1.0f)
{
	shader_program = resource_manager->load<gl::shader_program>("final.glsl");
	color_texture_input = shader_program->get_input("color_texture");
	bloom_texture_input = shader_program->get_input("bloom_texture");
	bloom_weight_input = shader_program->get_input("bloom_weight");
	blue_noise_texture_input = shader_program->get_input("blue_noise_texture");
	blue_noise_scale_input = shader_program->get_input("blue_noise_scale");
	resolution_input = shader_program->get_input("resolution");
	time_input = shader_program->get_input("time");

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

final_pass::~final_pass()
{
	delete quad_vao;
	delete quad_vbo;
}

void final_pass::render(const render::context& ctx, render::queue& queue) const
{
	rasterizer->use_framebuffer(*framebuffer);
	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	
	float2 resolution = {std::get<0>(viewport), std::get<1>(viewport)};

	// Change shader program
	rasterizer->use_program(*shader_program);

	// Upload shader parameters
	color_texture_input->upload(color_texture);
	if (bloom_texture && bloom_texture_input)
		bloom_texture_input->upload(bloom_texture);
	if (bloom_weight_input)
		bloom_weight_input->upload(bloom_weight);
	if (blue_noise_texture && blue_noise_texture_input)
		blue_noise_texture_input->upload(blue_noise_texture);
	if (blue_noise_scale_input)
		blue_noise_scale_input->upload(blue_noise_scale);
	if (resolution_input)
		resolution_input->upload(resolution);
	if (time_input)
		time_input->upload(ctx.t);

	// Draw quad
	rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
}

void final_pass::set_color_texture(const gl::texture_2d* texture)
{
	this->color_texture = texture;
}

void final_pass::set_bloom_texture(const gl::texture_2d* texture) noexcept
{
	this->bloom_texture = texture;
}

void final_pass::set_bloom_weight(float weight) noexcept
{
	this->bloom_weight = weight;
}

void final_pass::set_blue_noise_texture(const gl::texture_2d* texture)
{
	this->blue_noise_texture = texture;
	blue_noise_scale = 1.0f / static_cast<float>(texture->get_dimensions()[0]);
}

} // namespace render

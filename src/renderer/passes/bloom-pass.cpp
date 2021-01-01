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

#include "renderer/passes/bloom-pass.hpp"
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
#include "math/math.hpp"
#include <cmath>
#include <glad/glad.h>

bloom_pass::bloom_pass(::rasterizer* rasterizer, const ::framebuffer* framebuffer, resource_manager* resource_manager):
	render_pass(rasterizer, framebuffer),
	source_texture(nullptr),
	brightness_threshold(1.0f),
	blur_iterations(1)
{
	// Create clone of framebuffer texture
	const texture_2d* framebuffer_texture = framebuffer->get_color_attachment();
	auto dimensions = framebuffer_texture->get_dimensions();
	auto pixel_type = framebuffer_texture->get_pixel_type();
	auto pixel_format = framebuffer_texture->get_pixel_format();
	auto wrapping = framebuffer_texture->get_wrapping();
	auto filters = framebuffer_texture->get_filters();
	float max_anisotropy = framebuffer_texture->get_max_anisotropy();
	cloned_framebuffer_texture = new texture_2d(std::get<0>(dimensions), std::get<1>(dimensions), pixel_type, pixel_format);
	cloned_framebuffer_texture->set_wrapping(std::get<0>(wrapping), std::get<1>(wrapping));
	cloned_framebuffer_texture->set_filters(std::get<0>(filters), std::get<1>(filters));
	cloned_framebuffer_texture->set_max_anisotropy(max_anisotropy);
	
	// Create clone of framebuffer
	cloned_framebuffer = new ::framebuffer(std::get<0>(dimensions), std::get<1>(dimensions));
	cloned_framebuffer->attach(framebuffer_attachment_type::color, cloned_framebuffer_texture);
	
	// Setup pingponging
	pingpong_textures[0] = framebuffer_texture;
	pingpong_textures[1] = cloned_framebuffer_texture;
	pingpong_framebuffers[0] = framebuffer;
	pingpong_framebuffers[1] = cloned_framebuffer;	
	
	// Load brightness threshold shader
	threshold_shader = resource_manager->load<shader_program>("brightness-threshold.glsl");
	threshold_shader_image_input = threshold_shader->get_input("image");
	threshold_shader_resolution_input = threshold_shader->get_input("resolution");
	threshold_shader_threshold_input = threshold_shader->get_input("threshold");
	
	// Load blur shader
	blur_shader = resource_manager->load<shader_program>("blur.glsl");
	blur_shader_image_input = blur_shader->get_input("image");
	blur_shader_resolution_input = blur_shader->get_input("resolution");
	blur_shader_direction_input = blur_shader->get_input("direction");

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
}

bloom_pass::~bloom_pass()
{
	delete cloned_framebuffer;
	delete cloned_framebuffer_texture;
	delete quad_vao;
	delete quad_vbo;
}

void bloom_pass::render(render_context* context) const
{	
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Determine viewport based on framebuffer resolution
	auto viewport = framebuffer->get_dimensions();
	rasterizer->set_viewport(0, 0, std::get<0>(viewport), std::get<1>(viewport));
	float2 resolution = {std::get<0>(viewport), std::get<1>(viewport)};
	
	// Perform brightness threshold subpass, rendering to the first pingpong fbo
	rasterizer->use_framebuffer(*pingpong_framebuffers[0]);
	rasterizer->use_program(*threshold_shader);
	threshold_shader_image_input->upload(source_texture);
	threshold_shader_resolution_input->upload(resolution);
	threshold_shader_threshold_input->upload(brightness_threshold);
	rasterizer->draw_arrays(*quad_vao, drawing_mode::triangles, 0, 6);
	
	// Perform iterative blur subpass
	const float2 direction_horizontal = {1, 0};
	const float2 direction_vertical = {0, 1};
	rasterizer->use_program(*blur_shader);
	blur_shader_resolution_input->upload(resolution);
	for (int i = 0; i < blur_iterations; ++i)
	{
		// Perform horizontal blur
		rasterizer->use_framebuffer(*pingpong_framebuffers[1]);
		blur_shader_image_input->upload(pingpong_textures[0]);
		blur_shader_direction_input->upload(direction_horizontal);
		rasterizer->draw_arrays(*quad_vao, drawing_mode::triangles, 0, 6);
		
		// Perform vertical blur
		rasterizer->use_framebuffer(*pingpong_framebuffers[0]);
		blur_shader_image_input->upload(pingpong_textures[1]);
		blur_shader_direction_input->upload(direction_vertical);
		rasterizer->draw_arrays(*quad_vao, drawing_mode::triangles, 0, 6);
	}
}

void bloom_pass::set_source_texture(const texture_2d* texture)
{
	this->source_texture = texture;
}

void bloom_pass::set_brightness_threshold(float threshold)
{
	this->brightness_threshold = threshold;
}

void bloom_pass::set_blur_iterations(int iterations)
{
	this->blur_iterations = iterations;
}


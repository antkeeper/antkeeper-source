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

#include "render/passes/bloom-pass.hpp"
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
#include <algorithm>
#include <cmath>
#include <glad/glad.h>

namespace render {

bloom_pass::bloom_pass(gl::rasterizer* rasterizer, resource_manager* resource_manager):
	pass(rasterizer, nullptr),
	source_texture(nullptr),
	mip_chain_length(0),
	filter_radius(0.005f),
	corrected_filter_radius{filter_radius, filter_radius}
{
	// Load downsample shader with Karis average
	downsample_karis_shader = resource_manager->load<gl::shader_program>("bloom-downsample-karis.glsl");
	downsample_karis_source_texture_input = downsample_karis_shader->get_input("source_texture");
	
	// Load downsample shader
	downsample_shader = resource_manager->load<gl::shader_program>("bloom-downsample.glsl");
	downsample_source_texture_input = downsample_shader->get_input("source_texture");
	
	// Load upsample shader
	upsample_shader = resource_manager->load<gl::shader_program>("bloom-upsample.glsl");
	upsample_source_texture_input = upsample_shader->get_input("source_texture");
	upsample_filter_radius_input = upsample_shader->get_input("filter_radius");

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

bloom_pass::~bloom_pass()
{
	set_mip_chain_length(0);
	
	delete quad_vao;
	delete quad_vbo;
}

void bloom_pass::render(const render::context& ctx, render::queue& queue) const
{
	if (!source_texture || !mip_chain_length)
		return;
	
	// Disable depth testing
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	// Enable back-face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	// Disable blending
	glDisable(GL_BLEND);
	
	// Downsample first mip with Karis average
	{
		rasterizer->use_program(*downsample_karis_shader);
		downsample_karis_source_texture_input->upload(source_texture);
		
		rasterizer->use_framebuffer(*framebuffers[0]);
		rasterizer->set_viewport(0, 0, textures[0]->get_width(), textures[0]->get_height());
		
		rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
	}
	
	// Downsample remaining mips
	rasterizer->use_program(*downsample_shader);
	for (int i = 1; i < static_cast<int>(mip_chain_length); ++i)
	{
		rasterizer->use_framebuffer(*framebuffers[i]);
		rasterizer->set_viewport(0, 0, textures[i]->get_width(), textures[i]->get_height());
		
		// Use previous downsample texture as downsample source
		downsample_source_texture_input->upload(textures[i - 1]);
		
		rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
	}
	
	// Enable additive blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	
	// Upsample
	rasterizer->use_program(*upsample_shader);
	upsample_filter_radius_input->upload(corrected_filter_radius);
	for (int i = static_cast<int>(mip_chain_length) - 1; i > 0; --i)
	{
		const int j = i - 1;
		rasterizer->use_framebuffer(*framebuffers[j]);
		rasterizer->set_viewport(0, 0, textures[j]->get_width(), textures[j]->get_height());
		
		upsample_source_texture_input->upload(textures[i]);
		
		rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
	}
}

void bloom_pass::resize()
{
	unsigned int source_width = 1;
	unsigned int source_height = 1;
	if (source_texture)
	{
		// Get source texture dimensions
		source_width = source_texture->get_width();
		source_height = source_texture->get_height();
		
		// Correct filter radius according to source texture aspect ratio
		const float aspect_ratio = static_cast<float>(source_height) / static_cast<float>(source_width);
		corrected_filter_radius = {filter_radius * aspect_ratio, filter_radius};
	}
	
	// Resize mip chain
	for (unsigned int i = 0; i < mip_chain_length; ++i)
	{
		// Calculate mip dimensions
		unsigned int mip_width = std::max<unsigned int>(1, source_width >> (i + 1));
		unsigned int mip_height = std::max<unsigned int>(1, source_height >> (i + 1));
		
		// Resize mip texture
		textures[i]->resize(mip_width, mip_height, nullptr);
		
		// Resize mip framebuffer
		framebuffers[i]->resize({(int)mip_width, (int)mip_height});
	}
}

void bloom_pass::set_source_texture(const gl::texture_2d* texture)
{
	if (texture != source_texture)
	{
		if (texture)
		{
			if (source_texture)
			{
				if (texture->get_width() != source_texture->get_width() || texture->get_height() != source_texture->get_height())
				{
					source_texture = texture;
					resize();
				}
				else
				{
					source_texture = texture;
				}
			}
			else
			{
				source_texture = texture;
				resize();
			}
		}
		else
		{
			source_texture = texture;
		}
	}
}

void bloom_pass::set_mip_chain_length(unsigned int length)
{
	unsigned int source_width = 1;
	unsigned int source_height = 1;
	if (source_texture)
	{
		// Get source texture dimensions
		source_width = source_texture->get_width();
		source_height = source_texture->get_height();
	}
	
	if (length > mip_chain_length)
	{
		// Generate additional framebuffers
		for (unsigned int i = mip_chain_length; i < length; ++i)
		{
			// Calculate mip resolution
			unsigned int mip_width = std::max<unsigned int>(1, source_width >> (i + 1));
			unsigned int mip_height = std::max<unsigned int>(1, source_height >> (i + 1));
			
			// Generate mip texture
			gl::texture_2d* texture = new gl::texture_2d(mip_width, mip_height, gl::pixel_type::float_16, gl::pixel_format::rgb);
			texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
			texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
			texture->set_max_anisotropy(0.0f);
			textures.push_back(texture);
			
			// Generate mip framebuffer
			gl::framebuffer* framebuffer = new gl::framebuffer(mip_width, mip_height);
			framebuffer->attach(gl::framebuffer_attachment_type::color, texture);
			framebuffers.push_back(framebuffer);
		}
	}
	else if (length < mip_chain_length)
	{
		// Free excess framebuffers
		while (framebuffers.size() > length)
		{
			delete framebuffers.back();
			framebuffers.pop_back();
			
			delete textures.back();
			textures.pop_back();
		}
	}
	
	// Update mip chain length
	mip_chain_length = length;
}

void bloom_pass::set_filter_radius(float radius)
{
	filter_radius = radius;
	
	// Get aspect ratio of source texture
	float aspect_ratio = 1.0f;
	if (source_texture)
	{
		aspect_ratio = static_cast<float>(source_texture->get_height()) / static_cast<float>(source_texture->get_width());
	}
	
	// Correct filter radius according to source texture aspect ratio
	corrected_filter_radius = {filter_radius * aspect_ratio, filter_radius};
}

} // namespace render

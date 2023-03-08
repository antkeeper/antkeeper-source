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

#include <engine/render/passes/bloom-pass.hpp>
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
#include <engine/gl/texture-wrapping.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/render/context.hpp>
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
	// Load downsample shader template
	auto downsample_shader_template = resource_manager->load<gl::shader_template>("bloom-downsample.glsl");
	
	// Build downsample shader program with Karis averaging
	downsample_karis_shader = downsample_shader_template->build
	(
		{
			{"KARIS_AVERAGE", std::string()}
		}
	);
	
	// Build downsample shader program without Karis averaging
	downsample_shader = downsample_shader_template->build();
	
	// Load upsample shader template
	auto upsample_shader_template = resource_manager->load<gl::shader_template>("bloom-upsample.glsl");
	
	// Build upsample shader program
	upsample_shader = upsample_shader_template->build();
	
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

void bloom_pass::render(const render::context& ctx, render::queue& queue)
{
	// Execute command buffer
	for (const auto& command: command_buffer)
	{
		command();
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
				rebuild_command_buffer();
			}
		}
		else
		{
			source_texture = nullptr;
			rebuild_command_buffer();
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
			auto texture = std::make_unique<gl::texture_2d>(mip_width, mip_height, gl::pixel_type::float_16, gl::pixel_format::rgb);
			texture->set_wrapping(gl::texture_wrapping::extend, gl::texture_wrapping::extend);
			texture->set_filters(gl::texture_min_filter::linear, gl::texture_mag_filter::linear);
			texture->set_max_anisotropy(0.0f);
			
			// Generate mip framebuffer
			auto framebuffer = std::make_unique<gl::framebuffer>(mip_width, mip_height);
			framebuffer->attach(gl::framebuffer_attachment_type::color, texture.get());
			
			textures.push_back(std::move(texture));
			framebuffers.emplace_back(std::move(framebuffer));
		}
	}
	else if (length < mip_chain_length)
	{
		framebuffers.resize(length);
		textures.resize(length);
	}
	
	// Update mip chain length
	mip_chain_length = length;
	
	// Rebuild command buffer
	rebuild_command_buffer();
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

void bloom_pass::rebuild_command_buffer()
{
	command_buffer.clear();
	
	if (!source_texture ||
		!mip_chain_length ||
		!downsample_karis_shader ||
		!downsample_shader ||
		!upsample_shader)
	{
		return;
	}
	
	// Setup downsample state
	command_buffer.emplace_back
	(
		[]()
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glDisable(GL_BLEND);
		}
	);
	
	// Downsample first mip with Karis average
	if (auto source_texture_var = downsample_karis_shader->variable("source_texture"))
	{
		command_buffer.emplace_back
		(
			[&, source_texture_var]()
			{
				rasterizer->use_program(*downsample_karis_shader);
				rasterizer->use_framebuffer(*framebuffers[0]);
				rasterizer->set_viewport(0, 0, textures[0]->get_width(), textures[0]->get_height());
				
				source_texture_var->update(*source_texture);
				
				rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
			}
		);
	}
	
	// Downsample remaining mips
	if (mip_chain_length > 1)
	{
		if (auto source_texture_var = downsample_shader->variable("source_texture"))
		{
			command_buffer.emplace_back([&](){rasterizer->use_program(*downsample_shader);});
			
			for (int i = 1; i < static_cast<int>(mip_chain_length); ++i)
			{
				command_buffer.emplace_back
				(
					[&, source_texture_var, i]()
					{
						rasterizer->use_framebuffer(*framebuffers[i]);
						rasterizer->set_viewport(0, 0, textures[i]->get_width(), textures[i]->get_height());
						
						// Use previous downsample texture as downsample source
						source_texture_var->update(*textures[i - 1]);
						
						rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
					}
				);
			}
		}
	}
	
	// Setup upsample state
	command_buffer.emplace_back
	(
		[&]()
		{
			// Enable additive blending
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			glBlendEquation(GL_FUNC_ADD);
			
			// Bind upsample shader
			rasterizer->use_program(*upsample_shader);
		}
	);
	
	// Update upsample filter radius
	if (auto filter_radius_var = upsample_shader->variable("filter_radius"))
	{
		command_buffer.emplace_back([&, filter_radius_var](){filter_radius_var->update(corrected_filter_radius);});
	}
	
	// Upsample
	if (auto source_texture_var = upsample_shader->variable("source_texture"))
	{
		for (int i = static_cast<int>(mip_chain_length) - 1; i > 0; --i)
		{
			const int j = i - 1;
			
			command_buffer.emplace_back
			(
				[&, source_texture_var, i, j]()
				{
					rasterizer->use_framebuffer(*framebuffers[j]);
					rasterizer->set_viewport(0, 0, textures[j]->get_width(), textures[j]->get_height());
					
					source_texture_var->update(*textures[i]);
					
					rasterizer->draw_arrays(*quad_vao, gl::drawing_mode::triangles, 0, 6);
				}
			);
		}
	}
}

} // namespace render

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/passes/bloom-pass.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/gl/pipeline.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/render/context.hpp>
#include <algorithm>
#include <cmath>

using namespace hash::literals;

namespace render {

bloom_pass::bloom_pass(gl::pipeline* pipeline, resource_manager* resource_manager):
	pass(pipeline, nullptr)
{
	// Load downsample shader template
	auto downsample_shader_template = resource_manager->load<gl::shader_template>("bloom-downsample.glsl");
	
	// Build downsample shader program with Karis averaging
	m_downsample_karis_shader = downsample_shader_template->build
	(
		{
			{"KARIS_AVERAGE", std::string()}
		}
	);
	
	// Build downsample shader program without Karis averaging
	m_downsample_shader = downsample_shader_template->build();
	
	// Load upsample shader template
	auto upsample_shader_template = resource_manager->load<gl::shader_template>("bloom-upsample.glsl");
	
	// Build upsample shader program
	m_upsample_shader = upsample_shader_template->build();
	
	// Construct framebuffer texture sampler
	m_sampler = std::make_shared<gl::sampler>
	(
		gl::sampler_filter::linear,
		gl::sampler_filter::linear,
		gl::sampler_mipmap_mode::linear,
		gl::sampler_address_mode::clamp_to_edge,
		gl::sampler_address_mode::clamp_to_edge
	);
	
	// Allocate empty vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>();
}

void bloom_pass::render([[maybe_unused]] render::context& ctx)
{
	// Execute command buffer
	for (const auto& command: m_command_buffer)
	{
		command();
	}
}

void bloom_pass::set_source_texture(std::shared_ptr<gl::texture_2d> texture)
{
	if (m_source_texture != texture)
	{
		m_source_texture = texture;
		
		rebuild_mip_chain();
		correct_filter_radius();
		rebuild_command_buffer();
	}
}

void bloom_pass::set_mip_chain_length(unsigned int length)
{	
	m_mip_chain_length = length;
	rebuild_mip_chain();
	rebuild_command_buffer();
}

void bloom_pass::set_filter_radius(float radius)
{
	m_filter_radius = radius;
	correct_filter_radius();
}

void bloom_pass::rebuild_mip_chain()
{
	if (m_source_texture && m_mip_chain_length)
	{
		// Rebuild target image
		m_target_image = std::make_shared<gl::image_2d>
		(
			gl::format::r16g16b16_sfloat,
			m_source_texture->get_image_view()->get_image()->get_dimensions()[0],
			m_source_texture->get_image_view()->get_image()->get_dimensions()[1],
			m_mip_chain_length
		);
		
		m_target_textures.resize(m_mip_chain_length);
		m_target_framebuffers.resize(m_mip_chain_length);
		for (unsigned int i = 0; i < m_mip_chain_length; ++i)
		{
			// Rebuild mip texture
			m_target_textures[i] = std::make_shared<gl::texture_2d>
			(
				std::make_shared<gl::image_view_2d>
				(
					m_target_image,
					m_target_image->get_format(),
					i,
					1
				),
				m_sampler
			);
			
			// Rebuild mip framebuffer
			const gl::framebuffer_attachment attachments[1] =
			{{
				gl::color_attachment_bit,
				m_target_textures[i]->get_image_view(),
				0
			}};
			m_target_framebuffers[i] = std::make_shared<gl::framebuffer>
			(
				attachments,
				m_target_image->get_dimensions()[0] >> i,
				m_target_image->get_dimensions()[1] >> i
			);
		}
	}
	else
	{
		m_target_image = nullptr;
		m_target_textures.clear();
		m_target_framebuffers.clear();
	}
}

void bloom_pass::correct_filter_radius()
{
	// Get aspect ratio of target image
	float aspect_ratio = 1.0f;
	if (m_target_image)
	{
		aspect_ratio = static_cast<float>(m_target_image->get_dimensions()[1]) /
			static_cast<float>(m_target_image->get_dimensions()[0]);
	}
	
	// Correct filter radius according to target image aspect ratio
	m_corrected_filter_radius = {m_filter_radius * aspect_ratio, m_filter_radius};
}

void bloom_pass::rebuild_command_buffer()
{
	m_command_buffer.clear();
	
	if (!m_source_texture ||
		!m_mip_chain_length ||
		!m_downsample_karis_shader ||
		!m_downsample_shader ||
		!m_upsample_shader)
	{
		return;
	}
	
	// Setup downsample state
	m_command_buffer.emplace_back
	(
		[&]()
		{
			m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
			m_pipeline->bind_vertex_array(m_vertex_array.get());
			m_pipeline->set_depth_test_enabled(false);
			m_pipeline->set_cull_mode(gl::cull_mode::back);
			m_pipeline->set_color_blend_enabled(false);
		}
	);
	
	// Downsample first mip with Karis average
	if (auto source_texture_var = m_downsample_karis_shader->variable("source_texture"_fnv1a32))
	{
		m_command_buffer.emplace_back
		(
			[&, source_texture_var]()
			{
				m_pipeline->bind_shader_program(m_downsample_karis_shader.get());
				m_pipeline->bind_framebuffer(m_target_framebuffers[0].get());
				
				const auto& target_dimensions = m_target_image->get_dimensions();
				const gl::viewport viewport[1] = {{0.0f, 0.0f, static_cast<float>(target_dimensions[0]), static_cast<float>(target_dimensions[1])}};
				m_pipeline->set_viewport(0, viewport);
				
				source_texture_var->update(*m_source_texture);
				
				// Draw fullscreen triangle
				m_pipeline->draw(3, 1, 0, 0);
			}
		);
	}
	
	// Downsample remaining mips
	if (m_mip_chain_length > 1)
	{
		if (auto source_texture_var = m_downsample_shader->variable("source_texture"_fnv1a32))
		{
			m_command_buffer.emplace_back([&](){m_pipeline->bind_shader_program(m_downsample_shader.get());});
			
			for (int i = 1; i < static_cast<int>(m_mip_chain_length); ++i)
			{
				m_command_buffer.emplace_back
				(
					[&, source_texture_var, i]()
					{
						m_pipeline->bind_framebuffer(m_target_framebuffers[i].get());
						
						const auto& target_dimensions = m_target_image->get_dimensions();
						const gl::viewport viewport[1] = {{0.0f, 0.0f, static_cast<float>(target_dimensions[0] >> i), static_cast<float>(target_dimensions[1] >> i)}};
						m_pipeline->set_viewport(0, viewport);
						
						// Use previous downsample texture as downsample source
						source_texture_var->update(*m_target_textures[i - 1]);
						
						// Draw fullscreen triangle
						m_pipeline->draw(3, 1, 0, 0);
					}
				);
			}
		}
	}
	
	// Setup upsample state
	m_command_buffer.emplace_back
	(
		[&]()
		{
			// Enable additive blending
			m_pipeline->set_color_blend_enabled(true);
			m_pipeline->set_color_blend_equation
			({
				gl::blend_factor::one,
				gl::blend_factor::one,
				gl::blend_op::add,
				gl::blend_factor::one,
				gl::blend_factor::one,
				gl::blend_op::add
			});
			
			// Bind upsample shader
			m_pipeline->bind_shader_program(m_upsample_shader.get());
		}
	);
	
	// Update upsample filter radius
	if (auto filter_radius_var = m_upsample_shader->variable("filter_radius"_fnv1a32))
	{
		m_command_buffer.emplace_back([&, filter_radius_var](){filter_radius_var->update(m_corrected_filter_radius);});
	}
	
	// Upsample
	if (auto source_texture_var = m_upsample_shader->variable("source_texture"_fnv1a32))
	{
		for (int i = static_cast<int>(m_mip_chain_length) - 1; i > 0; --i)
		{
			const int j = i - 1;
			
			m_command_buffer.emplace_back
			(
				[&, source_texture_var, i, j]()
				{
					m_pipeline->bind_framebuffer(m_target_framebuffers[j].get());
					
					const auto& target_dimensions = m_target_image->get_dimensions();
					const gl::viewport viewport[1] = {{0.0f, 0.0f, static_cast<float>(target_dimensions[0] >> j), static_cast<float>(target_dimensions[1] >> j)}};
					m_pipeline->set_viewport(0, viewport);
					
					source_texture_var->update(*m_target_textures[i]);
					
					// Draw fullscreen triangle
					m_pipeline->draw(3, 1, 0, 0);
				}
			);
		}
	}
}

} // namespace render

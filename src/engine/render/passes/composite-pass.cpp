// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/passes/composite-pass.hpp>
#include <engine/gl/pipeline.hpp>
#include <engine/gl/framebuffer.hpp>
#include <engine/gl/shader-program.hpp>
#include <engine/gl/shader-variable.hpp>
#include <engine/gl/shader-template.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/texture.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/render/context.hpp>
#include <engine/debug/log.hpp>
#include <engine/hash/fnv.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::render
{
	using namespace hash::literals;

	composite_pass::composite_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resources::resource_manager* resource_manager):
		pass(pipeline, framebuffer)
	{
		// Construct empty vertex array
		m_vertex_array = std::make_unique<gl::vertex_array>();
	
		// Load shader template and build shader program
		auto shader_template = resource_manager->load<gl::shader_template>("composite.glsl");
		m_shader_program = shader_template->build();
		if (!m_shader_program->linked())
		{
			log_error("Failed to composite pass shader program: {}", m_shader_program->info());
			log_warning("{}", shader_template->configure(gl::shader_stage::vertex));
		}
	}

	void composite_pass::render(render::context& ctx)
	{
		// Update resolution
		const auto& viewport_dimensions = (m_framebuffer) ? m_framebuffer->dimensions() : m_pipeline->get_default_framebuffer_dimensions();
		m_resolution = {static_cast<float>(viewport_dimensions[0]), static_cast<float>(viewport_dimensions[1])};
	
		// Update time
		m_time = ctx.t;
	
		// Execute render commands
		for (const auto& command: m_command_buffer)
		{
			command();
		}
	
		// Increment current frame
		++m_frame;
	}

	void composite_pass::set_luminance_texture(std::shared_ptr<gl::texture_2d> texture)
	{
		m_luminance_texture = texture;
		rebuild_command_buffer();
	}

	void composite_pass::set_bloom_texture(std::shared_ptr<gl::texture_2d> texture) noexcept
	{
		m_bloom_texture = texture;
		rebuild_command_buffer();
	}

	void composite_pass::set_bloom_strength(float strength) noexcept
	{
		m_bloom_strength = strength;
	}

	void composite_pass::set_noise_texture(std::shared_ptr<gl::texture_2d> texture)
	{
		m_noise_texture = texture;
		rebuild_command_buffer();
	}

	void composite_pass::set_noise_strength(float strength) noexcept
	{
		m_noise_strength = strength;
	}

	void composite_pass::set_overlay_texture(std::shared_ptr<gl::texture_2d> texture)
	{
		m_overlay_texture = texture;
		rebuild_command_buffer();
	}

	void composite_pass::rebuild_command_buffer()
	{
		m_command_buffer.clear();
	
		m_command_buffer.emplace_back
		(
			[&]()
			{
				const gl::viewport viewport[1] = {{0.0f, 0.0f, m_resolution.x(), m_resolution.y()}};
			
				m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
				m_pipeline->set_viewport(0, viewport);
				m_pipeline->set_depth_test_enabled(false);
				m_pipeline->set_cull_mode(gl::cull_mode::back);
				m_pipeline->set_color_blend_enabled(false);
				m_pipeline->bind_framebuffer(m_framebuffer);
				m_pipeline->bind_shader_program(m_shader_program.get());
				m_pipeline->bind_vertex_array(m_vertex_array.get());
			}
		);
	
		if (m_luminance_texture)
		{
			if (const auto var = m_shader_program->variable("luminance_texture"_fnv1a32))
			{
				m_command_buffer.emplace_back([&, var](){var->update(*m_luminance_texture);});
			}
		}

		// Bloom
		if (m_bloom_texture)
		{
			if (const auto var = m_shader_program->variable("bloom_texture"_fnv1a32))
			{
				m_command_buffer.emplace_back([&, var](){var->update(*m_bloom_texture);});
			}
		}
		if (const auto var = m_shader_program->variable("bloom_strength"_fnv1a32))
		{
			m_command_buffer.emplace_back([&, var](){var->update(m_bloom_strength);});
		}

		// Noise
		if (m_noise_texture)
		{
			if (const auto var = m_shader_program->variable("noise_texture"_fnv1a32))
			{
				m_command_buffer.emplace_back([&, var](){var->update(*m_noise_texture);});
			}
		}
		if (const auto var = m_shader_program->variable("noise_strength"_fnv1a32))
		{
			m_command_buffer.emplace_back([&, var](){var->update(m_noise_strength);});
		}

		// Overlay
		if (m_overlay_texture)
		{
			if (const auto var = m_shader_program->variable("overlay_texture"_fnv1a32))
			{
				m_command_buffer.emplace_back([&, var](){var->update(*m_overlay_texture);});
			}
		}

		if (const auto var = m_shader_program->variable("resolution"_fnv1a32))
		{
			m_command_buffer.emplace_back([&, var](){var->update(m_resolution);});
		}
		if (const auto var = m_shader_program->variable("time"_fnv1a32))
		{
			m_command_buffer.emplace_back([&, var](){var->update(m_time);});
		}
		if (const auto frame_var = m_shader_program->variable("frame"_fnv1a32))
		{
			m_command_buffer.emplace_back([&, frame_var](){frame_var->update(m_frame);});
		}
	
		m_command_buffer.emplace_back
		(
			[&]()
			{
				// Draw fullscreen triangle
				m_pipeline->draw(3, 1, 0, 0);
			}
		);
	}
}

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/passes/final-pass.hpp>
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
#include <cmath>
#include <cstdint>
#include <engine/hash/fnv1a.hpp>
#include <engine/debug/log.hpp>

namespace render {

final_pass::final_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(pipeline, framebuffer)
{
	// Construct empty vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>();
	
	// Load shader template and build shader program
	auto shader_template = resource_manager->load<gl::shader_template>("final.glsl");
	m_shader_program = shader_template->build();
	if (!m_shader_program->linked())
	{
		debug::log_error("Failed to final pass shader program: {}", m_shader_program->info());
		debug::log_warning("{}", shader_template->configure(gl::shader_stage::vertex));
	}
}

void final_pass::render(render::context& ctx)
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

void final_pass::set_color_texture(std::shared_ptr<gl::texture_2d> texture)
{
	m_color_texture = texture;
	rebuild_command_buffer();
}

void final_pass::set_bloom_texture(std::shared_ptr<gl::texture_2d> texture) noexcept
{
	m_bloom_texture = texture;
	rebuild_command_buffer();
}

void final_pass::set_bloom_weight(float weight) noexcept
{
	m_bloom_weight = weight;
}

void final_pass::set_blue_noise_texture(std::shared_ptr<gl::texture_2d> texture)
{
	m_blue_noise_texture = texture;
	m_blue_noise_scale = 1.0f / static_cast<float>(texture->get_image_view()->get_image()->get_dimensions()[0]);
	rebuild_command_buffer();
}

void final_pass::rebuild_command_buffer()
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
	
	if (m_color_texture)
	{
		if (const auto var = m_shader_program->variable("color_texture"))
		{
			m_command_buffer.emplace_back([&, var](){var->update(*m_color_texture);});
		}
	}
	if (m_bloom_texture)
	{
		if (const auto var = m_shader_program->variable("bloom_texture"))
		{
			m_command_buffer.emplace_back([&, var](){var->update(*m_bloom_texture);});
		}
	}
	if (m_blue_noise_texture)
	{
		if (const auto var = m_shader_program->variable("blue_noise_texture"))
		{
			m_command_buffer.emplace_back([&, var](){var->update(*m_blue_noise_texture);});
		}
	}
	
	if (const auto var = m_shader_program->variable("bloom_weight"))
	{
		m_command_buffer.emplace_back([&, var](){var->update(m_bloom_weight);});
	}
	if (const auto var = m_shader_program->variable("blue_noise_scale"))
	{
		m_command_buffer.emplace_back([&, var](){var->update(m_blue_noise_scale);});
	}
	if (const auto var = m_shader_program->variable("resolution"))
	{
		m_command_buffer.emplace_back([&, var](){var->update(m_resolution);});
	}
	if (const auto var = m_shader_program->variable("time"))
	{
		m_command_buffer.emplace_back([&, var](){var->update(m_time);});
	}
	if (const auto frame_var = m_shader_program->variable("frame"))
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

} // namespace render

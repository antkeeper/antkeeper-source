// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/render/passes/resample-pass.hpp>
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

namespace render {

resample_pass::resample_pass(gl::pipeline* pipeline, const gl::framebuffer* framebuffer, resource_manager* resource_manager):
	pass(pipeline, framebuffer)
{
	// Construct empty vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>();
	
	// Load resample shader template
	auto shader_template = resource_manager->load<gl::shader_template>("resample.glsl");
	
	// Build resample shader program
	m_shader_program = shader_template->build();
	if (!m_shader_program->linked())
	{
		debug::log_error("Failed to build resample shader program: {}", m_shader_program->info());
		debug::log_warning("{}", shader_template->configure(gl::shader_stage::vertex));
	}
}

void resample_pass::render([[maybe_unused]] render::context& ctx)
{
	for (const auto& command: m_command_buffer)
	{
		command();
	}
}

void resample_pass::set_source_texture(std::shared_ptr<gl::texture_2d> texture)
{
	m_source_texture = texture;
	rebuild_command_buffer();
}

void resample_pass::rebuild_command_buffer()
{
	m_command_buffer.clear();
	
	if (!m_source_texture || !m_shader_program)
	{
		return;
	}
	
	// Setup resample state
	m_command_buffer.emplace_back
	(
		[&]()
		{
			const auto& viewport_dimensions = (m_framebuffer) ? m_framebuffer->dimensions() : m_pipeline->get_default_framebuffer_dimensions();
			const gl::viewport viewport[1] =
			{{
				0.0f,
				0.0f,
				static_cast<float>(viewport_dimensions[0]),
				static_cast<float>(viewport_dimensions[1])
			}};
			
			m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
			m_pipeline->set_viewport(0, viewport);
			m_pipeline->set_depth_test_enabled(false);
			m_pipeline->set_cull_mode(gl::cull_mode::back);
			
			m_pipeline->bind_framebuffer(m_framebuffer);
			m_pipeline->bind_shader_program(m_shader_program.get());
			m_pipeline->bind_vertex_array(m_vertex_array.get());
		}
	);
	
	// Update shader variables
	if (auto source_texture_var = m_shader_program->variable("source_texture"))
	{
		m_command_buffer.emplace_back([&, source_texture_var](){source_texture_var->update(*m_source_texture);});
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

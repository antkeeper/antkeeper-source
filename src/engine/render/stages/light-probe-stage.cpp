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

#include <engine/render/stages/light-probe-stage.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/scene/light-probe.hpp>
#include <engine/scene/collection.hpp>
#include <algorithm>
#include <execution>
#include <stdexcept>
#include <glad/glad.h>

namespace render {

light_probe_stage::light_probe_stage(gl::rasterizer& rasterizer, ::resource_manager& resource_manager):
	m_rasterizer(&rasterizer)
{
	// Build quad VBO and VAO
	{
		const math::vector2<float> vertex_positions[] =
		{
			{-1.0f,  1.0f},
			{-1.0f, -1.0f},
			{ 1.0f,  1.0f},
			{ 1.0f, -1.0f}
		};
		
		const auto vertex_data = std::as_bytes(std::span{vertex_positions});
		const std::size_t vertex_size = 2;
		const std::size_t vertex_stride = sizeof(float) * vertex_size;
		
		m_quad_vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data.size(), vertex_data);
		m_quad_vao = std::make_unique<gl::vertex_array>();
		
		// Define position vertex attribute
		gl::vertex_attribute position_attribute;
		position_attribute.buffer = m_quad_vbo.get();
		position_attribute.offset = 0;
		position_attribute.stride = vertex_stride;
		position_attribute.type = gl::vertex_attribute_type::float_32;
		position_attribute.components = 2;
		
		// Bind vertex attributes to VAO
		m_quad_vao->bind(render::vertex_attribute::position, position_attribute);
	}
	
	// Load cubemap to spherical harmonics shader template
	m_cubemap_to_sh_shader_template = resource_manager.load<gl::shader_template>("cubemap-to-sh.glsl");
	
	// Build cubemap to spherical harmonics shader program
	rebuild_cubemap_to_sh_shader_program();
}

void light_probe_stage::execute(render::context& ctx)
{
	// Get all light probes in the collection
	const auto& light_probes = ctx.collection->get_objects(scene::light_probe::object_type_id);
	
	bool state_bound = false;
	
	// For each light probe
	std::for_each
	(
		std::execution::seq,
		std::begin(light_probes),
		std::end(light_probes),
		[&](scene::object_base* object)
		{
			scene::light_probe& light_probe = static_cast<scene::light_probe&>(*object);
			if (!light_probe.is_illuminance_outdated() && !m_reproject_sh)
			{
				return;
			}
			
			// Setup viewport and bind cubemap to spherical harmonics shader program
			if (!state_bound)
			{
				glDisable(GL_BLEND);
				m_rasterizer->set_viewport(0, 0, 12, 1);
				m_rasterizer->use_program(*m_cubemap_to_sh_shader_program);
				state_bound = true;
			}
			
			// Bind light probe illuminance framebuffer
			m_rasterizer->use_framebuffer(*light_probe.get_illuminance_framebuffer());
			
			// Update cubemap to spherical harmonics cubemap variable with light probe luminance texture
			m_cubemap_var->update(*light_probe.get_luminance_texture());
			
			// Draw quad
			m_rasterizer->draw_arrays(*m_quad_vao, gl::drawing_mode::triangle_strip, 0, 4);
			
			// Mark light probe illuminance as current
			light_probe.set_illuminance_outdated(false);
		}
	);
	
	m_reproject_sh = false;
}

void light_probe_stage::set_sh_sample_count(std::size_t count)
{
	if (m_sh_sample_count != count)
	{
		m_sh_sample_count = count;
		sh_parameters_changed();
	}
}

void light_probe_stage::rebuild_cubemap_to_sh_shader_program()
{
	m_cubemap_to_sh_shader_program = m_cubemap_to_sh_shader_template->build({{"SAMPLE_COUNT", std::to_string(m_sh_sample_count)}});
	if (!m_cubemap_to_sh_shader_program->linked())
	{
		debug::log::error("Failed to build cubemap to spherical harmonics shader program: {}", m_cubemap_to_sh_shader_program->info());
		debug::log::warning("{}", m_cubemap_to_sh_shader_template->configure(gl::shader_stage::vertex));
		m_cubemap_var = nullptr;
		
		throw std::runtime_error("Failed to build cubemap to spherical harmonics shader program.");
	}
	else
	{
		m_cubemap_var = m_cubemap_to_sh_shader_program->variable("cubemap");
		if (!m_cubemap_var)
		{
			throw std::runtime_error("Cubemap to spherical harmonics shader program has no `cubemap` variable.");
		}
	}
}

void light_probe_stage::sh_parameters_changed()
{
	rebuild_cubemap_to_sh_shader_program();
	m_reproject_sh = true;
}

} // namespace render

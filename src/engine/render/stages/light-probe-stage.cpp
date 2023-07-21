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
		const math::fvec2 vertex_positions[] =
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
	
	// Load cubemap to spherical harmonics shader template and build shader program
	m_cubemap_to_sh_shader_template = resource_manager.load<gl::shader_template>("cubemap-to-sh.glsl");
	rebuild_cubemap_to_sh_shader_program();
	
	// Load cubemap downsample shader template and build shader program
	m_cubemap_downsample_shader_template = resource_manager.load<gl::shader_template>("cubemap-downsample.glsl");
	rebuild_cubemap_downsample_shader_program();
	
	// Load cubemap filter LUT shader template and build shader program
	m_cubemap_filter_lut_shader_template = resource_manager.load<gl::shader_template>("cubemap-filter-lut.glsl");
	rebuild_cubemap_filter_lut_shader_program();
	
	// Allocate cubemap filter LUT texture
	m_cubemap_filter_lut_texture = std::make_unique<gl::texture_2d>(static_cast<std::uint16_t>(m_cubemap_filter_sample_count), static_cast<std::uint16_t>(m_cubemap_filter_mip_count - 1), gl::pixel_type::float_32, gl::pixel_format::rgba);
	
	// Allocate cubemap filter LUT framebuffer and attach LUT texture
	m_cubemap_filter_lut_framebuffer = std::make_unique<gl::framebuffer>();
	m_cubemap_filter_lut_framebuffer->attach(gl::framebuffer_attachment_type::color, m_cubemap_filter_lut_texture.get());
	
	// Build cubemap filter LUT texture
	rebuild_cubemap_filter_lut_texture();
	
	// Load cubemap filter shader template and build shader program
	m_cubemap_filter_shader_template = resource_manager.load<gl::shader_template>("cubemap-filter.glsl");
	rebuild_cubemap_filter_shader_program();
}

void light_probe_stage::execute(render::context& ctx)
{
	const auto& light_probes = ctx.collection->get_objects(scene::light_probe::object_type_id);
	if (light_probes.empty())
	{
		return;
	}
	
	update_light_probes_luminance(light_probes);
	update_light_probes_illuminance(light_probes);
}

void light_probe_stage::update_light_probes_luminance(const std::vector<scene::object_base*>& light_probes)
{
	bool state_bound = false;
	
	// Downsample cubemaps
	std::for_each
	(
		std::execution::seq,
		std::begin(light_probes),
		std::end(light_probes),
		[&](scene::object_base* object)
		{
			scene::light_probe& light_probe = static_cast<scene::light_probe&>(*object);
			if (!light_probe.is_luminance_outdated() && !m_refilter_cubemaps)
			{
				return;
			}
			
			// Bind state, if unbound
			if (!state_bound)
			{
				glDisable(GL_BLEND);
				state_bound = true;
			}
			
			// Bind cubemap downsample shader program
			m_rasterizer->use_program(*m_cubemap_downsample_shader_program);
			
			// Update cubemap shader variable with light probe luminance texture
			m_cubemap_downsample_cubemap_var->update(*light_probe.get_luminance_texture());
			
			// Get resolution of cubemap face for base mip level
			const std::uint16_t base_mip_face_size = light_probe.get_luminance_texture()->get_face_size();
			
			// Downsample mip chain
			for (std::size_t i = 1; i < light_probe.get_luminance_framebuffers().size(); ++i)
			{
				// Set viewport to resolution of cubemap face size for current mip level
				const std::uint16_t current_mip_face_size = base_mip_face_size >> i;
				m_rasterizer->set_viewport(0, 0, current_mip_face_size, current_mip_face_size);
				
				// Restrict cubemap mipmap range to parent mip level
				const std::uint8_t parent_mip_level = static_cast<std::uint8_t>(i - 1);
				light_probe.get_luminance_texture()->set_mip_range(parent_mip_level, parent_mip_level);
				
				// Bind framebuffer of current cubemap mip level
				m_rasterizer->use_framebuffer(*light_probe.get_luminance_framebuffers()[i]);
				
				// Downsample
				m_rasterizer->draw_arrays(*m_quad_vao, gl::drawing_mode::points, 0, 1);
			}
			
			// Bind cubemap filter shader program
			m_rasterizer->use_program(*m_cubemap_filter_shader_program);
			
			// Pass cubemap and filter lut textures to cubemap filter shader program
			m_cubemap_filter_cubemap_var->update(*light_probe.get_luminance_texture());
			m_cubemap_filter_filter_lut_var->update(*m_cubemap_filter_lut_texture);
			
			// Filter mip chain
			for (int i = 1; i < static_cast<int>(light_probe.get_luminance_framebuffers().size()) - 2; ++i)
			{
				// Update mip level shader variable
				m_cubemap_filter_mip_level_var->update(static_cast<int>(i));
				
				// Set viewport to resolution of cubemap face size for current mip level
				const std::uint16_t current_mip_face_size = base_mip_face_size >> i;
				m_rasterizer->set_viewport(0, 0, current_mip_face_size, current_mip_face_size);
				
				// Restrict cubemap mipmap range to descendent levels
				light_probe.get_luminance_texture()->set_mip_range(static_cast<std::uint8_t>(i + 1), 255);
				
				// Bind framebuffer of current cubemap mip level
				m_rasterizer->use_framebuffer(*light_probe.get_luminance_framebuffers()[i]);
				
				// Filter
				m_rasterizer->draw_arrays(*m_quad_vao, gl::drawing_mode::points, 0, 1);
			}
			
			// Restore cubemap mipmap range
			light_probe.get_luminance_texture()->set_mip_range(0, 255);
			
			// Mark light probe luminance as current
			light_probe.set_luminance_outdated(false);
		}
	);
}

void light_probe_stage::update_light_probes_illuminance(const std::vector<scene::object_base*>& light_probes)
{
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
			m_cubemap_to_sh_cubemap_var->update(*light_probe.get_luminance_texture());
			
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

void light_probe_stage::set_cubemap_filter_sample_count(std::size_t count)
{
	if (m_cubemap_filter_sample_count != count)
	{
		m_cubemap_filter_sample_count = count;
		cubemap_filter_parameters_changed();
	}
}

void light_probe_stage::set_cubemap_filter_mip_bias(float bias)
{
	if (m_cubemap_filter_mip_bias != bias)
	{
		m_cubemap_filter_mip_bias = bias;
		cubemap_filter_parameters_changed();
	}
}

void light_probe_stage::rebuild_cubemap_to_sh_shader_program()
{
	m_cubemap_to_sh_shader_program = m_cubemap_to_sh_shader_template->build({{"SAMPLE_COUNT", std::to_string(m_sh_sample_count)}});
	if (!m_cubemap_to_sh_shader_program->linked())
	{
		debug::log::error("Failed to build cubemap to spherical harmonics shader program: {}", m_cubemap_to_sh_shader_program->info());
		debug::log::warning("{}", m_cubemap_to_sh_shader_template->configure(gl::shader_stage::vertex));
		m_cubemap_to_sh_cubemap_var = nullptr;
		
		throw std::runtime_error("Failed to build cubemap to spherical harmonics shader program.");
	}
	else
	{
		m_cubemap_to_sh_cubemap_var = m_cubemap_to_sh_shader_program->variable("cubemap");
		if (!m_cubemap_to_sh_cubemap_var)
		{
			throw std::runtime_error("Cubemap to spherical harmonics shader program has no `cubemap` variable.");
		}
	}
}

void light_probe_stage::rebuild_cubemap_downsample_shader_program()
{
	m_cubemap_downsample_shader_program = m_cubemap_downsample_shader_template->build({});
	if (!m_cubemap_downsample_shader_program->linked())
	{
		debug::log::error("Failed to build cubemap downsample shader program: {}", m_cubemap_downsample_shader_program->info());
		debug::log::warning("{}", m_cubemap_downsample_shader_template->configure(gl::shader_stage::vertex));
		m_cubemap_downsample_cubemap_var = nullptr;
		
		throw std::runtime_error("Failed to build cubemap downsample shader program.");
	}
	else
	{
		m_cubemap_downsample_cubemap_var = m_cubemap_downsample_shader_program->variable("cubemap");
		if (!m_cubemap_downsample_cubemap_var)
		{
			throw std::runtime_error("Cubemap downsample shader program has no `cubemap` variable.");
		}
	}
}

void light_probe_stage::rebuild_cubemap_filter_lut_shader_program()
{
	m_cubemap_filter_lut_shader_program = m_cubemap_filter_lut_shader_template->build({});
	if (!m_cubemap_filter_lut_shader_program->linked())
	{
		debug::log::error("Failed to build cubemap filter LUT shader program: {}", m_cubemap_filter_lut_shader_program->info());
		debug::log::warning("{}", m_cubemap_filter_lut_shader_template->configure(gl::shader_stage::vertex));
		m_cubemap_filter_lut_resolution_var = nullptr;
		m_cubemap_filter_lut_face_size_var = nullptr;
		m_cubemap_filter_lut_mip_bias_var = nullptr;
		
		throw std::runtime_error("Failed to build cubemap filter LUT shader program.");
	}
	else
	{
		m_cubemap_filter_lut_resolution_var = m_cubemap_filter_lut_shader_program->variable("resolution");
		m_cubemap_filter_lut_face_size_var = m_cubemap_filter_lut_shader_program->variable("face_size");
		m_cubemap_filter_lut_mip_bias_var = m_cubemap_filter_lut_shader_program->variable("mip_bias");
		if (!m_cubemap_filter_lut_resolution_var || !m_cubemap_filter_lut_face_size_var || !m_cubemap_filter_lut_mip_bias_var)
		{
			throw std::runtime_error("Cubemap filter LUT shader program is missing one or more required shader variables.");
		}
	}
}

void light_probe_stage::rebuild_cubemap_filter_lut_texture()
{
	glDisable(GL_BLEND);
	m_rasterizer->use_framebuffer(*m_cubemap_filter_lut_framebuffer);
	m_rasterizer->set_viewport(0, 0, m_cubemap_filter_lut_texture->get_width(), m_cubemap_filter_lut_texture->get_height());
	m_rasterizer->use_program(*m_cubemap_filter_lut_shader_program);
	m_cubemap_filter_lut_resolution_var->update(math::fvec2{static_cast<float>(m_cubemap_filter_lut_texture->get_width()), static_cast<float>(m_cubemap_filter_lut_texture->get_height())});
	m_cubemap_filter_lut_face_size_var->update(128.0f);
	m_cubemap_filter_lut_mip_bias_var->update(m_cubemap_filter_mip_bias);
	m_rasterizer->draw_arrays(*m_quad_vao, gl::drawing_mode::triangle_strip, 0, 4);
}

void light_probe_stage::rebuild_cubemap_filter_shader_program()
{
	m_cubemap_filter_shader_program = m_cubemap_filter_shader_template->build({{"SAMPLE_COUNT", std::to_string(m_cubemap_filter_sample_count)}});
	if (!m_cubemap_filter_shader_program->linked())
	{
		debug::log::error("Failed to build cubemap filter shader program: {}", m_cubemap_filter_shader_program->info());
		debug::log::warning("{}", m_cubemap_filter_shader_template->configure(gl::shader_stage::vertex));
		m_cubemap_filter_cubemap_var = nullptr;
		m_cubemap_filter_filter_lut_var = nullptr;
		m_cubemap_filter_mip_level_var = nullptr;
		
		throw std::runtime_error("Failed to build cubemap filter shader program.");
	}
	else
	{
		m_cubemap_filter_cubemap_var = m_cubemap_filter_shader_program->variable("cubemap");
		m_cubemap_filter_filter_lut_var = m_cubemap_filter_shader_program->variable("filter_lut");
		m_cubemap_filter_mip_level_var = m_cubemap_filter_shader_program->variable("mip_level");
		
		if (!m_cubemap_filter_cubemap_var || !m_cubemap_filter_filter_lut_var || !m_cubemap_filter_mip_level_var)
		{
			throw std::runtime_error("Cubemap filter shader program is missing one or more required shader variables.");
		}
	}
}

void light_probe_stage::sh_parameters_changed()
{
	rebuild_cubemap_to_sh_shader_program();
	m_reproject_sh = true;
}

void light_probe_stage::cubemap_filter_parameters_changed()
{
	m_refilter_cubemaps = true;
}

} // namespace render

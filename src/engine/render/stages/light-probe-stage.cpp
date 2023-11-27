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
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/scene/light-probe.hpp>
#include <engine/scene/collection.hpp>
#include <algorithm>
#include <execution>
#include <stdexcept>

namespace render {

light_probe_stage::light_probe_stage(gl::pipeline& pipeline, ::resource_manager& resource_manager):
	m_pipeline(&pipeline)
{
	// Generate restricted mip range samplers
	m_downsample_samplers.resize(16);
	m_filter_samplers.resize(16);
	for (std::size_t i = 0; i < m_downsample_samplers.size(); ++i)
	{
		m_downsample_samplers[i] = std::make_shared<gl::sampler>
		(
			gl::sampler_filter::linear,
			gl::sampler_filter::linear,
			gl::sampler_mipmap_mode::linear,
			gl::sampler_address_mode::repeat,
			gl::sampler_address_mode::repeat,
			gl::sampler_address_mode::repeat,
			0.0f,
			0.0f,
			false,
			gl::compare_op::less,
			static_cast<float>(i),
			static_cast<float>(i)
		);
		
		m_filter_samplers[i] = std::make_shared<gl::sampler>
		(
			gl::sampler_filter::linear,
			gl::sampler_filter::linear,
			gl::sampler_mipmap_mode::linear,
			gl::sampler_address_mode::repeat,
			gl::sampler_address_mode::repeat,
			gl::sampler_address_mode::repeat,
			0.0f,
			0.0f,
			false,
			gl::compare_op::less,
			static_cast<float>(i),
			1000.0f
		);
	}
	
	// Construct empty vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>();
	
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
	m_cubemap_filter_lut_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r32g32b32a32_sfloat,
				static_cast<std::uint32_t>(m_cubemap_filter_sample_count),
				static_cast<std::uint32_t>(m_cubemap_filter_mip_count - 1)
			)
		),
		std::make_shared<gl::sampler>
		(
			gl::sampler_filter::nearest,
			gl::sampler_filter::nearest,
			gl::sampler_mipmap_mode::nearest,
			gl::sampler_address_mode::clamp_to_edge,
			gl::sampler_address_mode::clamp_to_edge
		)
	);
	
	// Allocate cubemap filter LUT framebuffer and attach LUT texture
	const gl::framebuffer_attachment attachments[1] =
	{{
		gl::color_attachment_bit,
		m_cubemap_filter_lut_texture->get_image_view(),
		0
	}};
	m_cubemap_filter_lut_framebuffer = std::make_unique<gl::framebuffer>(attachments, m_cubemap_filter_lut_texture->get_image_view()->get_image()->get_dimensions()[0], m_cubemap_filter_lut_texture->get_image_view()->get_image()->get_dimensions()[1]);
	
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
			
			// Store light probe luminance sampler
			auto light_probe_luminance_sampler = light_probe.get_luminance_texture()->get_sampler();
			
			// Bind state, if unbound
			if (!state_bound)
			{
				m_pipeline->set_primitive_topology(gl::primitive_topology::point_list);
				m_pipeline->bind_vertex_array(m_vertex_array.get());
				m_pipeline->set_color_blend_enabled(false);
				state_bound = true;
			}
			
			// Bind cubemap downsample shader program
			m_pipeline->bind_shader_program(m_cubemap_downsample_shader_program.get());
			
			// Get resolution of cubemap face for base mip level
			const auto base_mip_face_size = light_probe.get_luminance_texture()->get_image_view()->get_image()->get_dimensions()[0];
			
			// Downsample mip chain
			for (std::size_t i = 1; i < light_probe.get_luminance_framebuffers().size(); ++i)
			{
				// Set viewport to resolution of cubemap face size for current mip level
				const auto current_mip_face_size = base_mip_face_size >> i;
				const gl::viewport viewport[1] =
				{{
					0,
					0,
					static_cast<float>(current_mip_face_size),
					static_cast<float>(current_mip_face_size)
				}};
				m_pipeline->set_viewport(0, viewport);
				
				// Restrict cubemap mipmap range to parent mip level
				light_probe.get_luminance_texture()->set_sampler(m_downsample_samplers[i - 1]);
				
				// Update cubemap shader variable with light probe luminance texture
				m_cubemap_downsample_cubemap_var->update(*light_probe.get_luminance_texture());
				
				// Bind framebuffer of current cubemap mip level
				m_pipeline->bind_framebuffer(light_probe.get_luminance_framebuffers()[i].get());
				
				// Downsample
				m_pipeline->draw(1, 1, 0, 0);
			}
			
			// Bind cubemap filter shader program
			m_pipeline->bind_shader_program(m_cubemap_filter_shader_program.get());
			
			// Pass filter lut texture to cubemap filter shader program
			m_cubemap_filter_filter_lut_var->update(*m_cubemap_filter_lut_texture);
			
			// Filter mip chain
			for (int i = 1; i < static_cast<int>(light_probe.get_luminance_framebuffers().size()) - 2; ++i)
			{
				// Update mip level shader variable
				m_cubemap_filter_mip_level_var->update(static_cast<int>(i));
				
				// Set viewport to resolution of cubemap face size for current mip level
				const auto current_mip_face_size = base_mip_face_size >> i;
				const gl::viewport viewport[1] =
				{{
					0,
					0,
					static_cast<float>(current_mip_face_size),
					static_cast<float>(current_mip_face_size)
				}};
				m_pipeline->set_viewport(0, viewport);
				
				// Restrict cubemap mipmap range to descendent levels
				light_probe.get_luminance_texture()->set_sampler(m_filter_samplers[i + 1]);
				
				// Update cubemap shader variable with light probe luminance texture
				m_cubemap_filter_cubemap_var->update(*light_probe.get_luminance_texture());
				
				// Bind framebuffer of current cubemap mip level
				m_pipeline->bind_framebuffer(light_probe.get_luminance_framebuffers()[i].get());
				
				// Filter
				m_pipeline->draw(1, 1, 0, 0);
			}
			
			// Restore light probe luminance sampler
			light_probe.get_luminance_texture()->set_sampler(light_probe_luminance_sampler);
			
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
				m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
				m_pipeline->bind_vertex_array(m_vertex_array.get());
				m_pipeline->set_color_blend_enabled(false);
				
				const gl::viewport viewport[1] =
				{{
					0,
					0,
					12,
					1
				}};
				m_pipeline->set_viewport(0, viewport);
				
				m_pipeline->bind_shader_program(m_cubemap_to_sh_shader_program.get());
				state_bound = true;
			}
			
			// Bind light probe illuminance framebuffer
			m_pipeline->bind_framebuffer(light_probe.get_illuminance_framebuffer().get());
			
			// Update cubemap to spherical harmonics cubemap variable with light probe luminance texture
			m_cubemap_to_sh_cubemap_var->update(*light_probe.get_luminance_texture());
			
			// Draw fullscreen triangle
			m_pipeline->draw(3, 1, 0, 0);
			
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
		debug::log_error("Failed to build cubemap to spherical harmonics shader program: {}", m_cubemap_to_sh_shader_program->info());
		debug::log_warning("{}", m_cubemap_to_sh_shader_template->configure(gl::shader_stage::vertex));
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
		debug::log_error("Failed to build cubemap downsample shader program: {}", m_cubemap_downsample_shader_program->info());
		debug::log_warning("{}", m_cubemap_downsample_shader_template->configure(gl::shader_stage::vertex));
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
		debug::log_error("Failed to build cubemap filter LUT shader program: {}", m_cubemap_filter_lut_shader_program->info());
		debug::log_warning("{}", m_cubemap_filter_lut_shader_template->configure(gl::shader_stage::vertex));
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
	m_pipeline->set_color_blend_enabled(false);
	m_pipeline->bind_framebuffer(m_cubemap_filter_lut_framebuffer.get());
	
	const auto& cubemap_filter_lut_dimensions = m_cubemap_filter_lut_texture->get_image_view()->get_image()->get_dimensions();
	const gl::viewport viewport[1] =
	{{
		0,
		0,
		static_cast<float>(cubemap_filter_lut_dimensions[0]),
		static_cast<float>(cubemap_filter_lut_dimensions[1])
	}};
	m_pipeline->set_viewport(0, viewport);
	
	m_pipeline->bind_shader_program(m_cubemap_filter_lut_shader_program.get());
	m_cubemap_filter_lut_resolution_var->update(math::fvec2{static_cast<float>(cubemap_filter_lut_dimensions[0]), static_cast<float>(cubemap_filter_lut_dimensions[1])});
	m_cubemap_filter_lut_face_size_var->update(128.0f);
	m_cubemap_filter_lut_mip_bias_var->update(m_cubemap_filter_mip_bias);
	m_cubemap_filter_lut_framebuffer->resize(cubemap_filter_lut_dimensions[0], cubemap_filter_lut_dimensions[1]);
	
	m_pipeline->bind_vertex_array(m_vertex_array.get());
	m_pipeline->set_primitive_topology(gl::primitive_topology::triangle_list);
	m_pipeline->draw(3, 1, 0, 0);
}

void light_probe_stage::rebuild_cubemap_filter_shader_program()
{
	m_cubemap_filter_shader_program = m_cubemap_filter_shader_template->build({{"SAMPLE_COUNT", std::to_string(m_cubemap_filter_sample_count)}});
	if (!m_cubemap_filter_shader_program->linked())
	{
		debug::log_error("Failed to build cubemap filter shader program: {}", m_cubemap_filter_shader_program->info());
		debug::log_warning("{}", m_cubemap_filter_shader_template->configure(gl::shader_stage::vertex));
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

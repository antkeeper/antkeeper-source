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

#include <engine/scene/light-probe.hpp>

namespace scene {

light_probe::light_probe()
{
	// Allocate illuminance texture
	m_illuminance_texture = std::make_shared<gl::texture_1d>
	(
		std::make_shared<gl::image_view_1d>
		(
			std::make_shared<gl::image_1d>
			(
				gl::format::r32g32b32a32_sfloat,
				12
			)
		),
		std::make_shared<gl::sampler>
		(
			gl::sampler_filter::nearest,
			gl::sampler_filter::nearest,
			gl::sampler_mipmap_mode::nearest,
			gl::sampler_address_mode::clamp_to_edge
		)
	);
	
	// Allocate and init illuminance framebuffer
	const gl::framebuffer_attachment attachments[1] =
	{
		{
			gl::color_attachment_bit,
			m_illuminance_texture->get_image_view(),
			0
		}
	};
	m_illuminance_framebuffer = std::make_shared<gl::framebuffer>(attachments, 12, 1);
	
	// Init illuminance matrices
	m_illuminance_matrices[0] = {};
	m_illuminance_matrices[1] = {};
	m_illuminance_matrices[2] = {};
}

void light_probe::update_illuminance_matrices()
{
	m_illuminance_texture->get_image_view()->get_image()->read
	(
		0,
		0,
		0,
		0,
		12,
		1,
		1,
		gl::format::r32g32b32a32_sfloat,
		std::as_writable_bytes(std::span{m_illuminance_matrices})
	);
}

void light_probe::set_luminance_texture(std::shared_ptr<gl::texture_cube> texture)
{
	if (m_luminance_texture != texture)
	{
		m_luminance_texture = texture;
		
		// Update luminance framebuffers
		if (m_luminance_texture)
		{
			const auto face_size = texture->get_image_view()->get_image()->get_dimensions()[0];
			const auto mip_count = static_cast<std::uint32_t>(std::bit_width(face_size));
			
			m_luminance_framebuffers.resize(mip_count);
			
			for (std::uint32_t i = 0; i < mip_count; ++i)
			{
				const gl::framebuffer_attachment attachments[1] =
				{
					{
						gl::color_attachment_bit,
						m_luminance_texture->get_image_view(),
						i
					}
				};
				m_luminance_framebuffers[i] = std::make_shared<gl::framebuffer>(attachments, face_size >> i, face_size >> i);
			}
		}
		else
		{
			m_luminance_framebuffers.clear();
		}
		
		set_luminance_outdated(true);
		set_illuminance_outdated(true);
	}
}

void light_probe::set_luminance_outdated(bool outdated)
{
	m_luminance_outdated = outdated;
}

void light_probe::set_illuminance_outdated(bool outdated)
{
	m_illuminance_outdated = outdated;
}

void light_probe::transformed()
{
	m_bounds = {get_translation(), get_translation()};
}

} // namespace scene

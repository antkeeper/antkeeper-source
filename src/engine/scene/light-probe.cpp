// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.light_probe;
import engine.debug.contract;

namespace engine::scene
{
	namespace
	{
		// 4 floats per pixel to encode the 3 4x4 spherical harmonics illuminance matrices in a 12 pixel 1D image.
		inline constexpr gl::format light_probe_illuminance_image_format = gl::format::r32g32b32a32_sfloat;

		// 12 pixels, corresponding to the column vectors of three spherical harmonics illuminance matrices (R0, R1, R2, R3, G0, G1, G2, G3, B0, B1, B2, B3).
		inline constexpr u32 light_probe_illuminance_image_width = 12;
	}

	light_probe::light_probe(gl::format format, u32 face_size):
		light_probe()
	{
		const auto mip_levels = static_cast<u32>(std::bit_width(face_size));

		set_luminance_texture
		(
			std::make_shared<gl::texture_cube>
			(
				// Luminance texture image view
				std::make_shared<gl::image_view_cube>
				(
					std::make_shared<gl::image_cube>
					(
						format,
						face_size,
						mip_levels
					),
					gl::format::undefined,
					0,
					mip_levels
				),

				// Lumiance texture sampler
				std::make_shared<gl::sampler>
				(
					gl::sampler_filter::linear,
					gl::sampler_filter::linear,
					gl::sampler_mipmap_mode::linear,
					gl::sampler_address_mode::clamp_to_edge,
					gl::sampler_address_mode::clamp_to_edge
				)
			)
		);
	}

	light_probe::light_probe()
	{
		// Allocate illuminance texture
		m_illuminance_texture = std::make_shared<gl::texture_1d>
		(
			std::make_shared<gl::image_view_1d>
			(
				std::make_shared<gl::image_1d>
				(
					light_probe_illuminance_image_format,
					light_probe_illuminance_image_width
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
		m_illuminance_framebuffer = std::make_shared<gl::framebuffer>(attachments, light_probe_illuminance_image_width, 1);
	
		// Init illuminance matrices
		m_illuminance_matrices[0] = {};
		m_illuminance_matrices[1] = {};
		m_illuminance_matrices[2] = {};
	}

	void light_probe::update_illuminance_matrices()
	{
		debug::precondition(m_illuminance_texture != nullptr);

		m_illuminance_texture->get_image_view()->get_image()->read
		(
			0,
			0,
			0,
			0,
			light_probe_illuminance_image_width,
			1,
			1,
			light_probe_illuminance_image_format,
			std::as_writable_bytes(std::span{m_illuminance_matrices})
		);
	}

	void light_probe::set_luminance_texture(std::shared_ptr<gl::texture_cube> texture)
	{
		if (m_luminance_texture == texture)
		{
			return;
		}

		m_luminance_texture = texture;
		
		// Update luminance framebuffers
		if (m_luminance_texture)
		{
			debug::invariant(m_luminance_texture->get_image_view() != nullptr);
			debug::invariant(m_luminance_texture->get_image_view()->get_image() != nullptr);

			const auto face_size = m_luminance_texture->get_image_view()->get_image()->get_dimensions()[0];
			const auto mip_count = static_cast<u32>(std::bit_width(face_size));
			
			m_luminance_framebuffers.resize(mip_count);
			
			for (u32 i = 0; i < mip_count; ++i)
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
}

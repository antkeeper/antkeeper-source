// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
import engine.gl.image_view;
import engine.gl.opengl.format_lut;
import <stdexcept>;

namespace engine::gl
{
	image_view::image_view
	(
		std::shared_ptr<gl::image> image,
		u8 dimensionality,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer,
		u32 array_layer_count,
		u8 flags
	)
	{
		if (!image)
		{
			throw std::invalid_argument("Image view has null image.");
		}

		if (format == gl::format::undefined)
		{
			format = image->get_format();
		}

		const auto format_index = std::to_underlying(format);
		const auto gl_internal_format = opengl::format_lut[format_index][0];

		if (!gl_internal_format)
		{
			throw std::invalid_argument("Image view has unsupported format.");
		}

		if (!mip_level_count)
		{
			throw std::invalid_argument("Image view has zero mip levels.");
		}

		if (first_mip_level + mip_level_count > image->get_mip_levels())
		{
			throw std::out_of_range("Image view mip range out of image mip range.");
		}

		if (!array_layer_count)
		{
			throw std::invalid_argument("Image view has zero array layers.");
		}

		if (first_array_layer + array_layer_count > image->get_array_layers())
		{
			throw std::out_of_range("Image view array layer range out of image array layer range.");
		}

		if (dimensionality != image->get_dimensionality())
		{
			throw std::invalid_argument("Image view dimensionality must match image dimensionality.");
		}

		if (flags & std::to_underlying(image_view_flag::cube))
		{
			if (!image->is_cube_compatible())
			{
				throw std::invalid_argument("Cube image views must be constructed from cube-compatible images.");
			}

			if (array_layer_count % 6 != 0)
			{
				throw std::invalid_argument("Cube image views array layer count must be a multiple of 6.");
			}
		}

		m_image = image;
		m_dimensionality = dimensionality;
		m_format = format;
		m_first_mip_level = first_mip_level;
		m_mip_level_count = mip_level_count;
		m_first_array_layer = first_array_layer;
		m_array_layer_count = array_layer_count;
		m_flags = flags;

		unsigned int gl_target = 0;
		switch (dimensionality)
		{
			case 1:
				gl_target = is_array() ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
				break;

			case 2:
				if (is_cube())
				{
					gl_target = is_array() ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
				}
				else
				{
					gl_target = is_array() ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
				}
				break;

			case 3:
				gl_target = GL_TEXTURE_3D;
				break;

			default:
				break;
		}

		glGenTextures(1, &m_gl_texture_name);
		glTextureView
		(
			m_gl_texture_name,
			gl_target,
			m_image->m_gl_texture_name,
			gl_internal_format,
			m_first_mip_level,
			m_mip_level_count,
			m_first_array_layer,
			m_array_layer_count
		);
	}

	image_view::~image_view()
	{
		glDeleteTextures(1, &m_gl_texture_name);
	}

	image_view_1d::image_view_1d
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer
	):
		image_view
		(
			image,
			1,
			format,
			first_mip_level,
			mip_level_count,
			first_array_layer,
			1,
			0
		)
	{
	}

	image_view_1d_array::image_view_1d_array
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer,
		u32 array_layer_count
	):
		image_view
		(
			image,
			1,
			format,
			first_mip_level,
			mip_level_count,
			first_array_layer,
			array_layer_count,
			std::to_underlying(image_view_flag::array)
		)
	{
	}

	image_view_2d::image_view_2d
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer
	):
		image_view
		(
			image,
			2,
			format,
			first_mip_level,
			mip_level_count,
			first_array_layer,
			1,
			0
		)
	{
	}

	image_view_2d_array::image_view_2d_array
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer,
		u32 array_layer_count
	):
		image_view
		(
			image,
			2,
			format,
			first_mip_level,
			mip_level_count,
			first_array_layer,
			array_layer_count,
			std::to_underlying(image_view_flag::array)
		)
	{
	}

	image_view_3d::image_view_3d
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count
	):
		image_view
		(
			image,
			3,
			format,
			first_mip_level,
			mip_level_count,
			0,
			1,
			0
		)
	{
	}

	image_view_cube::image_view_cube
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer
	):
		image_view
		(
			image,
			2,
			format,
			first_mip_level,
			mip_level_count,
			first_array_layer,
			6,
			std::to_underlying(image_view_flag::cube)
		)
	{
	}

	image_view_cube_array::image_view_cube_array
	(
		std::shared_ptr<gl::image> image,
		gl::format format,
		u32 first_mip_level,
		u32 mip_level_count,
		u32 first_array_layer,
		u32 array_layer_count
	):
		image_view
		(
			image,
			2,
			format,
			first_mip_level,
			mip_level_count,
			first_array_layer,
			array_layer_count,
			std::to_underlying(image_view_flag::array) | std::to_underlying(image_view_flag::cube)
		)
	{
	}
}

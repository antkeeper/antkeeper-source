// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
import engine.gl.vertex_array;
import engine.gl.opengl.format_lut;
import engine.utility.sized_types;
import <stdexcept>;
import <utility>;

namespace engine::gl
{	
	// 0 = unscaled, 1 = normalized, 2 = scaled
	static constexpr u8 format_scale_lut[] =
	{
		0, // undefined
		1, // r4g4_unorm_pack8
		1, // r4g4b4a4_unorm_pack16
		1, // b4g4r4a4_unorm_pack16
		1, // r5g6b5_unorm_pack16
		1, // b5g6r5_unorm_pack16
		1, // r5g5b5a1_unorm_pack16
		1, // b5g5r5a1_unorm_pack16
		1, // a1r5g5b5_unorm_pack16
		1, // r8_unorm
		1, // r8_snorm
		2, // r8_uscaled
		2, // r8_sscaled
		0, // r8_uint
		0, // r8_sint
		0, // r8_srgb
		1, // r8g8_unorm
		1, // r8g8_snorm
		2, // r8g8_uscaled
		2, // r8g8_sscaled
		0, // r8g8_uint
		0, // r8g8_sint
		0, // r8g8_srgb
		1, // r8g8b8_unorm
		1, // r8g8b8_snorm
		2, // r8g8b8_uscaled
		2, // r8g8b8_sscaled
		0, // r8g8b8_uint
		0, // r8g8b8_sint
		0, // r8g8b8_srgb
		1, // b8g8r8_unorm
		1, // b8g8r8_snorm
		2, // b8g8r8_uscaled
		2, // b8g8r8_sscaled
		0, // b8g8r8_uint
		0, // b8g8r8_sint
		0, // b8g8r8_srgb
		1, // r8g8b8a8_unorm
		1, // r8g8b8a8_snorm
		2, // r8g8b8a8_uscaled
		2, // r8g8b8a8_sscaled
		0, // r8g8b8a8_uint
		0, // r8g8b8a8_sint
		0, // r8g8b8a8_srgb
		1, // b8g8r8a8_unorm
		1, // b8g8r8a8_snorm
		2, // b8g8r8a8_uscaled
		2, // b8g8r8a8_sscaled
		0, // b8g8r8a8_uint
		0, // b8g8r8a8_sint
		0, // b8g8r8a8_srgb
		1, // a8b8g8r8_unorm_pack32
		1, // a8b8g8r8_snorm_pack32
		2, // a8b8g8r8_uscaled_pack32
		2, // a8b8g8r8_sscaled_pack32
		0, // a8b8g8r8_uint_pack32
		0, // a8b8g8r8_sint_pack32
		0, // a8b8g8r8_srgb_pack32
		1, // a2r10g10b10_unorm_pack32
		1, // a2r10g10b10_snorm_pack32
		2, // a2r10g10b10_uscaled_pack32
		2, // a2r10g10b10_sscaled_pack32
		0, // a2r10g10b10_uint_pack32
		0, // a2r10g10b10_sint_pack32
		1, // a2b10g10r10_unorm_pack32
		1, // a2b10g10r10_snorm_pack32
		2, // a2b10g10r10_uscaled_pack32
		2, // a2b10g10r10_sscaled_pack32
		0, // a2b10g10r10_uint_pack32
		0, // a2b10g10r10_sint_pack32
		1, // r16_unorm
		1, // r16_snorm
		2, // r16_uscaled
		2, // r16_sscaled
		0, // r16_uint
		0, // r16_sint
		0, // r16_sfloat
		1, // r16g16_unorm
		1, // r16g16_snorm
		2, // r16g16_uscaled
		2, // r16g16_sscaled
		0, // r16g16_uint
		0, // r16g16_sint
		0, // r16g16_sfloat
		1, // r16g16b16_unorm
		1, // r16g16b16_snorm
		2, // r16g16b16_uscaled
		2, // r16g16b16_sscaled
		0, // r16g16b16_uint
		0, // r16g16b16_sint
		0, // r16g16b16_sfloat
		1, // r16g16b16a16_unorm
		1, // r16g16b16a16_snorm
		2, // r16g16b16a16_uscaled
		2, // r16g16b16a16_sscaled
		0, // r16g16b16a16_uint
		0, // r16g16b16a16_sint
		0, // r16g16b16a16_sfloat
		0, // r32_uint
		0, // r32_sint
		0, // r32_sfloat
		0, // r32g32_uint
		0, // r32g32_sint
		0, // r32g32_sfloat
		0, // r32g32b32_uint
		0, // r32g32b32_sint
		0, // r32g32b32_sfloat
		0, // r32g32b32a32_uint
		0, // r32g32b32a32_sint
		0, // r32g32b32a32_sfloat
		0, // r64_uint
		0, // r64_sint
		0, // r64_sfloat
		0, // r64g64_uint
		0, // r64g64_sint
		0, // r64g64_sfloat
		0, // r64g64b64_uint
		0, // r64g64b64_sint
		0, // r64g64b64_sfloat
		0, // r64g64b64a64_uint
		0, // r64g64b64a64_sint
		0, // r64g64b64a64_sfloat
		0, // b10g11r11_ufloat_pack32
		0, // e5b9g9r9_ufloat_pack32
		1, // d16_unorm
		1, // x8_d24_unorm_pack32
		0, // d32_sfloat
		0, // s8_uint
		1, // d16_unorm_s8_uint
		1, // d24_unorm_s8_uint
		0, // d32_sfloat_s8_uint
		1, // bc1_rgb_unorm_block,
		0, // bc1_rgb_srgb_block,
		1, // bc1_rgba_unorm_block,
		0, // bc1_rgba_srgb_block,
		1, // bc2_unorm_block,
		0, // bc2_srgb_block,
		1, // bc3_unorm_block,
		0, // bc3_srgb_block,
		1, // bc4_unorm_block,
		1, // bc4_snorm_block,
		1, // bc5_unorm_block,
		1, // bc5_snorm_block,
		2, // bc6h_ufloat_block,
		2, // bc6h_sfloat_block,
		1, // bc7_unorm_block,
		0, // bc7_srgb_block,
		1, // etc2_r8g8b8_unorm_block,
		0, // etc2_r8g8b8_srgb_block,
		1, // etc2_r8g8b8a1_unorm_block,
		0, // etc2_r8g8b8a1_srgb_block,
		1, // etc2_r8g8b8a8_unorm_block,
		0, // etc2_r8g8b8a8_srgb_block,
		1, // eac_r11_unorm_block,
		1, // eac_r11_snorm_block,
		1, // eac_r11g11_unorm_block,
		1, // eac_r11g11_snorm_block,
		1, // astc_4x4_unorm_block,
		0, // astc_4x4_srgb_block,
		1, // astc_5x4_unorm_block,
		0, // astc_5x4_srgb_block,
		1, // astc_5x5_unorm_block,
		0, // astc_5x5_srgb_block,
		1, // astc_6x5_unorm_block,
		0, // astc_6x5_srgb_block,
		1, // astc_6x6_unorm_block,
		0, // astc_6x6_srgb_block,
		1, // astc_8x5_unorm_block,
		0, // astc_8x5_srgb_block,
		1, // astc_8x6_unorm_block,
		0, // astc_8x6_srgb_block,
		1, // astc_8x8_unorm_block,
		0, // astc_8x8_srgb_block,
		1, // astc_10x5_unorm_block,
		0, // astc_10x5_srgb_block,
		1, // astc_10x6_unorm_block,
		0, // astc_10x6_srgb_block,
		1, // astc_10x8_unorm_block,
		0, // astc_10x8_srgb_block,
		1, // astc_10x10_unorm_block,
		0, // astc_10x10_srgb_block,
		1, // astc_12x10_unorm_block,
		0, // astc_12x10_srgb_block,
		1, // astc_12x12_unorm_block,
		0, // astc_12x12_srgb_block
	};

	vertex_array::vertex_array(std::span<const vertex_input_attribute> attributes)
	{
		m_attributes.assign(attributes.begin(), attributes.end());

		glCreateVertexArrays(1, &m_gl_named_array);

		for (const auto& attribute : m_attributes)
		{
			// Enable attribute
			glEnableVertexArrayAttrib(m_gl_named_array, static_cast<GLuint>(attribute.location));

			// Set attribute vertex binding index
			glVertexArrayAttribBinding
			(
				m_gl_named_array,
				static_cast<GLuint>(attribute.location),
				static_cast<GLuint>(attribute.binding)
			);

			const auto format_index = std::to_underlying(attribute.format);
			const auto gl_base_format = opengl::format_lut[format_index][1];
			const auto gl_type = opengl::format_lut[format_index][2];
			const auto format_scale = format_scale_lut[format_index];

			// Determine number of values per vertex
			GLint gl_size;
			switch (gl_base_format)
			{
				case GL_RED:
				case GL_RED_INTEGER:
				case GL_DEPTH_COMPONENT:
				case GL_STENCIL_INDEX:
					gl_size = 1;
					break;

				case GL_RG:
				case GL_RG_INTEGER:
				case GL_DEPTH_STENCIL:
					gl_size = 2;
					break;

				case GL_BGR:
				case GL_BGR_INTEGER:
				case GL_RGB:
				case GL_RGB_INTEGER:
					gl_size = 3;
					break;

				case GL_BGRA:
				case GL_BGRA_INTEGER:
				case GL_RGBA:
				case GL_RGBA_INTEGER:
					gl_size = 4;
					break;

				default:
					gl_size = 0;
					break;
			}

			if (gl_size == 0 || gl_type == 0)
			{
				throw std::invalid_argument("Vertex input attribute has unsupported format.");
			}

			if (format_scale > 0 || gl_type == GL_FLOAT || gl_type == GL_HALF_FLOAT)
			{
				glVertexArrayAttribFormat
				(
					m_gl_named_array,
					static_cast<GLuint>(attribute.location),
					gl_size,
					gl_type,
					(format_scale == 1),
					static_cast<GLuint>(attribute.offset)
				);
			}
			else if (gl_type == GL_DOUBLE)
			{
				glVertexArrayAttribLFormat
				(
					m_gl_named_array,
					static_cast<GLuint>(attribute.location),
					gl_size,
					gl_type,
					static_cast<GLuint>(attribute.offset)
				);
			}
			else
			{
				glVertexArrayAttribIFormat
				(
					m_gl_named_array,
					static_cast<GLuint>(attribute.location),
					gl_size,
					gl_type,
					static_cast<GLuint>(attribute.offset)
				);
			}
		}
	}

	vertex_array::vertex_array()
	{
		glCreateVertexArrays(1, &m_gl_named_array);
	}

	vertex_array::~vertex_array()
	{
		glDeleteVertexArrays(1, &m_gl_named_array);
	}
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <glad/gl.h>
#include <stb/stb_image.h>
#include <tinyexr.h>
#include <cstring>
#include <engine/gl/image.hpp>
#include <engine/gl/cube-map.hpp>
#include <engine/gl/opengl/format-lut.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/debug/log.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/math/functions.hpp>
#include <stdexcept>

namespace engine::gl
{
	image::image
	(
		u8 dimensionality,
		gl::format format,
		u32 width,
		u32 height,
		u32 depth,
		u32 mip_levels,
		u32 array_layers,
		u32 flags
	)
	{
		const auto format_index = std::to_underlying(format);
		const auto gl_internal_format = opengl::format_lut[format_index][0];
		const auto gl_type = opengl::format_lut[format_index][2];

		if (gl_internal_format == 0 || gl_type == 0)
		{
			throw std::invalid_argument("Image construction used unsupported format.");
		}

		if (!width || !height || !depth)
		{
			throw std::invalid_argument("Image dimensions must be nonzero.");
		}

		if (!mip_levels)
		{
			throw std::invalid_argument("Image mip levels must be nonzero.");
		}

		if (mip_levels > static_cast<u32>(std::bit_width(math::max(math::max(width, height), depth))))
		{
			throw std::out_of_range("Image mip levels exceed `1 + log2(max(width, height, depth))`.");
		}

		if (!array_layers)
		{
			throw std::invalid_argument("Image array layers must be nonzero.");
		}

		if (dimensionality == 1)
		{
			if (height > 1 || depth > 1)
			{
				throw std::invalid_argument("1D image must have a height and depth of `1`.");
			}
		}
		else if (dimensionality == 2)
		{
			if (depth > 1)
			{
				throw std::invalid_argument("2D image must have a depth of `1`.");
			}
		}
		else if (dimensionality == 3)
		{
			if (array_layers > 1)
			{
				throw std::invalid_argument("3D image arrays not supported.");
			}
		}

		if (flags & std::to_underlying(image_flag::cube_compatible))
		{
			if (dimensionality != 2)
			{
				throw std::invalid_argument("Cube compatible image must be 2D.");
			}

			if (width != height)
			{
				throw std::invalid_argument("Cube compatible image width and height must be equal.");
			}

			if (array_layers % 6 != 0)
			{
				throw std::invalid_argument("Cube compatible image array layers must be a multiple of 6.");
			}
		}

		m_dimensionality = dimensionality;
		m_format = format;
		m_dimensions = {width, height, depth};
		m_mip_levels = mip_levels;
		m_array_layers = array_layers;
		m_flags = flags;

		if (m_array_layers == 1)
		{
			switch (m_dimensionality)
			{
				case 1:
					m_gl_texture_target = GL_TEXTURE_1D;
					glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
					glTextureStorage1D
					(
						m_gl_texture_name,
						static_cast<GLsizei>(m_mip_levels),
						gl_internal_format,
						static_cast<GLsizei>(m_dimensions[0])
					);
					break;

				case 2:
					m_gl_texture_target = GL_TEXTURE_2D;
					glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
					glTextureStorage2D
					(
						m_gl_texture_name,
						static_cast<GLsizei>(m_mip_levels),
						gl_internal_format,
						static_cast<GLsizei>(m_dimensions[0]),
						static_cast<GLsizei>(m_dimensions[1])
					);
					break;

				case 3:
					m_gl_texture_target = GL_TEXTURE_3D;
					glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
					glTextureStorage3D
					(
						m_gl_texture_name,
						static_cast<GLsizei>(m_mip_levels),
						gl_internal_format,
						static_cast<GLsizei>(m_dimensions[0]),
						static_cast<GLsizei>(m_dimensions[1]),
						static_cast<GLsizei>(m_dimensions[2])
					);
					break;

				default:
					break;
			}
		}
		else
		{
			switch (m_dimensionality)
			{
				case 1:
					m_gl_texture_target = GL_TEXTURE_1D_ARRAY;
					glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
					glTextureStorage2D
					(
						m_gl_texture_name,
						static_cast<GLsizei>(m_mip_levels),
						gl_internal_format,
						static_cast<GLsizei>(m_dimensions[0]),
						static_cast<GLsizei>(m_array_layers)
					);
					break;

				case 2:
					if (is_cube_compatible())
					{
						if (m_array_layers == 6)
						{
							m_gl_texture_target = GL_TEXTURE_CUBE_MAP;
							glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
							glTextureStorage2D
							(
								m_gl_texture_name,
								static_cast<GLsizei>(m_mip_levels),
								gl_internal_format,
								static_cast<GLsizei>(m_dimensions[0]),
								static_cast<GLsizei>(m_dimensions[1])
							);
						}
						else
						{
							m_gl_texture_target = GL_TEXTURE_CUBE_MAP_ARRAY;
							glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
							glTextureStorage3D
							(
								m_gl_texture_name,
								static_cast<GLsizei>(m_mip_levels),
								gl_internal_format,
								static_cast<GLsizei>(m_dimensions[0]),
								static_cast<GLsizei>(m_dimensions[1]),
								static_cast<GLsizei>(m_array_layers)
							);
						}
					}
					else
					{
						m_gl_texture_target = GL_TEXTURE_2D_ARRAY;
						glCreateTextures(m_gl_texture_target, 1, &m_gl_texture_name);
						glTextureStorage3D
						(
							m_gl_texture_name,
							static_cast<GLsizei>(m_mip_levels),
							gl_internal_format,
							static_cast<GLsizei>(m_dimensions[0]),
							static_cast<GLsizei>(m_dimensions[1]),
							static_cast<GLsizei>(m_array_layers)
						);
					}
					break;

				default:
					break;
			}
		}
	}

	image::~image()
	{
		glDeleteTextures(1, &m_gl_texture_name);
	}

	void image::read
	(
		u32 mip_level,
		u32 offset_x,
		u32 offset_y,
		u32 offset_z,
		u32 width,
		u32 height,
		u32 depth,
		gl::format format,
		std::span<std::byte> data
	) const
	{
		if (mip_level >= m_mip_levels)
		{
			throw std::out_of_range("Image read operation mip level out of range.");
		}

		const auto format_index = std::to_underlying(format);
		const auto gl_base_format = opengl::format_lut[format_index][1];
		const auto gl_type = opengl::format_lut[format_index][2];

		if (gl_base_format == 0 || gl_type == 0)
		{
			throw std::invalid_argument("Image read operation used unsupported format.");
		}

		glGetTextureSubImage
		(
			m_gl_texture_name,
			static_cast<GLint>(mip_level),
			static_cast<GLint>(offset_x),
			static_cast<GLint>(offset_y),
			static_cast<GLint>(offset_z),
			static_cast<GLsizei>(width),
			static_cast<GLsizei>(height),
			static_cast<GLsizei>(depth),
			gl_base_format,
			gl_type,
			static_cast<GLsizei>(data.size()),
			data.data()
		);
	}

	void image::write
	(
		u32 mip_level,
		u32 offset_x,
		u32 offset_y,
		u32 offset_z,
		u32 width,
		u32 height,
		u32 depth,
		gl::format format,
		std::span<const std::byte> data
	)
	{
		if (mip_level >= m_mip_levels)
		{
			throw std::out_of_range("Image write operation mip level out of range.");
		}

		const auto format_index = std::to_underlying(format);
		const auto gl_base_format = opengl::format_lut[format_index][1];
		const auto gl_type = opengl::format_lut[format_index][2];

		if (gl_base_format == 0 || gl_type == 0)
		{
			throw std::invalid_argument("Image write operation used unsupported format.");
		}

		if (m_array_layers == 1)
		{
			if ((offset_x + width > math::max(1_u32, m_dimensions[0] >> mip_level)) ||
				(offset_y + height > math::max(1_u32, m_dimensions[1] >> mip_level)) ||
				(offset_z + depth > math::max(1_u32, m_dimensions[2] >> mip_level)))
			{
				throw std::out_of_range("Image write operation exceeded image bounds.");
			}

			switch (m_dimensionality)
			{
				case 1:
					glTextureSubImage1D
					(
						m_gl_texture_name,
						static_cast<GLint>(mip_level),
						static_cast<GLint>(offset_x),
						static_cast<GLsizei>(width),
						gl_base_format,
						gl_type,
						data.data()
					);
					break;

				case 2:
					glTextureSubImage2D
					(
						m_gl_texture_name,
						static_cast<GLint>(mip_level),
						static_cast<GLint>(offset_x),
						static_cast<GLint>(offset_y),
						static_cast<GLsizei>(width),
						static_cast<GLsizei>(height),
						gl_base_format,
						gl_type,
						data.data()
					);
					break;

				case 3:
					glTextureSubImage3D
					(
						m_gl_texture_name,
						static_cast<GLint>(mip_level),
						static_cast<GLint>(offset_x),
						static_cast<GLint>(offset_y),
						static_cast<GLint>(offset_z),
						static_cast<GLsizei>(width),
						static_cast<GLsizei>(height),
						static_cast<GLsizei>(depth),
						gl_base_format,
						gl_type,
						data.data()
					);
					break;

				default:
					break;
			}
		}
		else
		{
			switch (m_dimensionality)
			{
				case 1:
					if ((offset_x + width > math::max(1_u32, m_dimensions[0] >> mip_level)) ||
						(offset_y + height > m_array_layers) ||
						(offset_z + depth > 1))
					{
						throw std::out_of_range("Image write operation exceeded image dimensions.");
					}

					glTextureSubImage2D
					(
						m_gl_texture_name,
						static_cast<GLint>(mip_level),
						static_cast<GLint>(offset_x),
						static_cast<GLint>(offset_y),
						static_cast<GLsizei>(width),
						static_cast<GLsizei>(height),
						gl_base_format,
						gl_type,
						data.data()
					);
					break;

				case 2:
					if ((offset_x + width > math::max(1_u32, m_dimensions[0] >> mip_level)) ||
						(offset_y + height > math::max(1_u32, m_dimensions[1] >> mip_level)) ||
						(offset_z + depth > m_array_layers))
					{
						throw std::out_of_range("Image write operation exceeded image bounds.");
					}

					glTextureSubImage3D
					(
						m_gl_texture_name,
						static_cast<GLint>(mip_level),
						static_cast<GLint>(offset_x),
						static_cast<GLint>(offset_y),
						static_cast<GLint>(offset_z),
						static_cast<GLsizei>(width),
						static_cast<GLsizei>(height),
						static_cast<GLsizei>(depth),
						gl_base_format,
						gl_type,
						data.data()
					);
					break;

				default:
					break;
			}
		}
	}

	void image::copy
	(
		u32 src_mip_level,
		u32 src_x,
		u32 src_y,
		u32 src_z,
		image& dst_image,
		u32 dst_mip_level,
		u32 dst_x,
		u32 dst_y,
		u32 dst_z,
		u32 width,
		u32 height,
		u32 depth
	) const
	{
		glCopyImageSubData
		(
			m_gl_texture_name,
			m_gl_texture_target,
			static_cast<GLint>(src_mip_level),
			static_cast<GLint>(src_x),
			static_cast<GLint>(src_y),
			static_cast<GLint>(src_z),
			dst_image.m_gl_texture_name,
			dst_image.m_gl_texture_target,
			static_cast<GLint>(dst_mip_level),
			static_cast<GLint>(dst_x),
			static_cast<GLint>(dst_y),
			static_cast<GLint>(dst_z),
			static_cast<GLsizei>(width),
			static_cast<GLsizei>(height),
			static_cast<GLsizei>(depth)
		);
	}

	void image::generate_mipmaps()
	{
		if (m_mip_levels > 1)
		{
			glGenerateTextureMipmap(m_gl_texture_name);
		}
	}

	image_1d::image_1d
	(
		gl::format format,
		u32 width,
		u32 mip_levels,
		u32 array_layers,
		u32 flags
	):
		image
		(
			1,
			format,
			width,
			1,
			1,
			mip_levels,
			array_layers,
			flags
		)
	{
	}

	image_2d::image_2d
	(
		gl::format format,
		u32 width,
		u32 height,
		u32 mip_levels,
		u32 array_layers,
		u32 flags
	):
		image
		(
			2,
			format,
			width,
			height,
			1,
			mip_levels,
			array_layers,
			flags
		)
	{
	}

	image_3d::image_3d
	(
		gl::format format,
		u32 width,
		u32 height,
		u32 depth,
		u32 mip_levels,
		u32 flags
	):
		image
		(
			3,
			format,
			width,
			height,
			depth,
			mip_levels,
			1,
			flags
		)
	{
	}

	image_cube::image_cube
	(
		gl::format format,
		u32 width,
		u32 mip_levels,
		u32 array_layers
	):
		image_2d
		(
			format,
			width,
			width,
			mip_levels,
			array_layers,
			std::to_underlying(image_flag::cube_compatible)
		)
	{
	}
}

namespace engine::resources
{
	int stb_io_read(void* user, char* data, int size)
	{
		deserialize_context& ctx = *static_cast<deserialize_context*>(user);
		return static_cast<int>(ctx.read8(reinterpret_cast<std::byte*>(data), static_cast<usize>(size)));
	}

	void stb_io_skip(void* user, int n)
	{
		deserialize_context& ctx = *static_cast<deserialize_context*>(user);
		ctx.seek(ctx.tell() + n);
	}

	int stb_io_eof(void* user)
	{
		deserialize_context& ctx = *static_cast<deserialize_context*>(user);
		return static_cast<int>(ctx.eof());
	}

	struct stb_image_deleter
	{
		void operator()(void* p) const
		{
			stbi_image_free(p);
		}
	};

	[[nodiscard]] std::unique_ptr<gl::image> load_image_stb_image(deserialize_context& ctx, u8 dimensionality, u32 mip_levels)
	{
		// Setup IO callbacks
		const stbi_io_callbacks io_callbacks
		{
			&stb_io_read,
			&stb_io_skip,
			&stb_io_eof
		};

		// Determine image bit depth
		usize component_size = stbi_is_16_bit_from_callbacks(&io_callbacks, &ctx) ? sizeof(u16) : sizeof(u8);
		ctx.seek(0);

		// Set vertical flip on load in order to correctly upload pixel data to OpenGL
		stbi_set_flip_vertically_on_load(true);

		// Load image data
		std::unique_ptr<void, stb_image_deleter> data;
		int width;
		int height;
		int components;
		gl::format format;
		if (component_size == sizeof(u16))
		{
			// Load 16-bit image data
			data = std::unique_ptr<void, stb_image_deleter>(stbi_load_16_from_callbacks(&io_callbacks, &ctx, &width, &height, &components, 0));

			// Determine 16-bit image format
			format = [components]()
				{
					switch (components)
					{
						case 1:
							return gl::format::r16_unorm;
						case 2:
							return gl::format::r16g16_unorm;
						case 3:
							return gl::format::r16g16b16_unorm;
						case 4:
							return gl::format::r16g16b16a16_unorm;
						default:
							return gl::format::undefined;
					}
				}();
		}
		else
		{
			// Load 8-bit image data
			data = std::unique_ptr<void, stb_image_deleter>(stbi_load_from_callbacks(&io_callbacks, &ctx, &width, &height, &components, 0));

			// Determine 8-bit image format
			format = [components]()
				{
					switch (components)
					{
						case 1:
							return gl::format::r8_unorm;
						case 2:
							return gl::format::r8g8_unorm;
						case 3:
							return gl::format::r8g8b8_unorm;
						case 4:
							return gl::format::r8g8b8a8_unorm;
						default:
							return gl::format::undefined;
					}
				}();
		}

		// Check if image data was loaded
		if (!data)
		{
			throw deserialize_error(stbi_failure_reason());
		}

		// Determine number mip levels
		if (!mip_levels)
		{
			mip_levels = static_cast<u32>(std::bit_width(static_cast<u32>(math::max(width, height))));
		}

		// Allocate image
		std::unique_ptr<gl::image> image;
		switch (dimensionality)
		{
			case 1:
				image = std::make_unique<gl::image_1d>
					(
						format,
						static_cast<u32>(math::max(width, height)),
						mip_levels
					);
				break;

			case 2:
				image = std::make_unique<gl::image_2d>
					(
						format,
						static_cast<u32>(width),
						static_cast<u32>(height),
						mip_levels
					);
				break;

			case 3:
				image = std::make_unique<gl::image_3d>
					(
						format,
						static_cast<u32>(width),
						static_cast<u32>(height),
						1,
						mip_levels
					);
				break;

			default:
				break;
		}

		// Upload image data to image
		image->write
		(
			0,
			0,
			0,
			0,
			image->get_dimensions()[0],
			image->get_dimensions()[1],
			image->get_dimensions()[2],
			format,
			{
				reinterpret_cast<const std::byte*>(data.get()),
				image->get_dimensions()[0] *
					image->get_dimensions()[1] *
					image->get_dimensions()[2] *
					static_cast<usize>(components) *
					component_size
			}
		);

		// Generate mipmaps
		image->generate_mipmaps();

		return image;
	}

	[[nodiscard]] std::unique_ptr<gl::image> load_image_tinyexr(deserialize_context& ctx, u8 dimensionality, u32 mip_levels)
	{
		const char* error = nullptr;
		auto tinyexr_error = [&error]()
			{
				const std::string error_message(error);
				FreeEXRErrorMessage(error);
				throw deserialize_error(error_message);
			};

		// Read data into file buffer
		std::vector<unsigned char> file_buffer(ctx.size());
		ctx.read8(reinterpret_cast<std::byte*>(file_buffer.data()), file_buffer.size());

		// Read EXR version
		EXRVersion exr_version;
		if (ParseEXRVersionFromMemory(&exr_version, file_buffer.data(), file_buffer.size()) != TINYEXR_SUCCESS)
		{
			tinyexr_error();
		}

		// Check if image is multipart
		if (exr_version.multipart)
		{
			throw deserialize_error("OpenEXR multipart images not supported.");
		}

		// Load image header
		EXRHeader exr_header;
		InitEXRHeader(&exr_header);
		if (ParseEXRHeaderFromMemory(&exr_header, &exr_version, file_buffer.data(), file_buffer.size(), &error) != TINYEXR_SUCCESS)
		{
			tinyexr_error();
		}

		// Check if image is tiled
		if (exr_header.tiled)
		{
			FreeEXRHeader(&exr_header);
			throw deserialize_error("OpenEXR tiled images not supported.");
		}

		// Check if image has a supported number of channels
		if (exr_header.num_channels < 1 || exr_header.num_channels > 4)
		{
			FreeEXRHeader(&exr_header);
			throw deserialize_error("OpenEXR images must have 1-4 channels.");
		}

		// Check if all channels have the same format
		for (int i = 1; i < exr_header.num_channels; ++i)
		{
			if (exr_header.pixel_types[i] != exr_header.pixel_types[i - 1])
			{
				FreeEXRHeader(&exr_header);
				throw deserialize_error("OpenEXR images must have the same pixel type per channel.");
			}
		}

		// Load image data
		EXRImage exr_image;
		InitEXRImage(&exr_image);
		if (LoadEXRImageFromMemory(&exr_image, &exr_header, file_buffer.data(), file_buffer.size(), &error) != TINYEXR_SUCCESS)
		{
			FreeEXRHeader(&exr_header);
			tinyexr_error();
		}

		// Free file buffer
		file_buffer.clear();

		// Determine image format
		constexpr gl::format uint_formats[4] =
		{
			gl::format::r32_uint,
			gl::format::r32g32_uint,
			gl::format::r32g32b32_uint,
			gl::format::r32g32b32a32_uint
		};
		constexpr gl::format half_formats[4] =
		{
			gl::format::r16_sfloat,
			gl::format::r16g16_sfloat,
			gl::format::r16g16b16_sfloat,
			gl::format::r16g16b16a16_sfloat
		};
		constexpr gl::format float_formats[4] =
		{
			gl::format::r32_sfloat,
			gl::format::r32g32_sfloat,
			gl::format::r32g32b32_sfloat,
			gl::format::r32g32b32a32_sfloat
		};
		gl::format format;
		int component_size;
		switch (exr_header.pixel_types[0])
		{
			case TINYEXR_PIXELTYPE_UINT:
				format = uint_formats[exr_header.num_channels - 1];
				component_size = static_cast<int>(sizeof(u32));
				break;

			case TINYEXR_PIXELTYPE_HALF:
				format = half_formats[exr_header.num_channels - 1];
				component_size = static_cast<int>(sizeof(u16));//sizeof(float16_t)
				break;

			case TINYEXR_PIXELTYPE_FLOAT:
				format = float_formats[exr_header.num_channels - 1];
				component_size = static_cast<int>(sizeof(float));//sizeof(float32_t)
				break;

			default:
				format = gl::format::undefined;
				component_size = 0;
				break;
		}

		// Allocate interleaved image data
		std::vector<std::byte> data(static_cast<usize>(exr_image.width * exr_image.height * exr_header.num_channels * component_size));

		// Interleave image data from layers
		std::byte* component = data.data();
		for (auto y = exr_image.height - 1; y >= 0; --y)
		{
			const auto row_offset = y * exr_image.width;

			for (auto x = 0; x < exr_image.width; ++x)
			{
				const auto byte_offset = (row_offset + x) * component_size;

				for (auto c = exr_image.num_channels - 1; c >= 0; --c)
				{
					std::memcpy(component, exr_image.images[c] + byte_offset, static_cast<usize>(component_size));
					component += component_size;
				}
			}
		}

		// Store image dimensions
		const auto width = static_cast<u32>(exr_image.width);
		const auto height = static_cast<u32>(exr_image.height);

		// Free loaded image data and image header
		FreeEXRImage(&exr_image);
		FreeEXRHeader(&exr_header);

		// Determine number mip levels
		if (!mip_levels)
		{
			mip_levels = static_cast<u32>(std::bit_width(math::max(width, height)));
		}

		// Allocate image
		std::unique_ptr<gl::image> image;
		switch (dimensionality)
		{
			case 1:
				image = std::make_unique<gl::image_1d>
					(
						format,
						math::max(width, height),
						mip_levels
					);
				break;

			case 2:
				image = std::make_unique<gl::image_2d>
					(
						format,
						width,
						height,
						mip_levels
					);
				break;

			case 3:
				image = std::make_unique<gl::image_3d>
					(
						format,
						width,
						height,
						1,
						mip_levels
					);
				break;

			default:
				break;
		}

		// Upload interleaved image data to image
		image->write
		(
			0,
			0,
			0,
			0,
			image->get_dimensions()[0],
			image->get_dimensions()[1],
			image->get_dimensions()[2],
			format,
			data
		);

		// Generate mipmaps
		image->generate_mipmaps();

		return image;
	}

	[[nodiscard]] std::unique_ptr<gl::image> load_image(deserialize_context& ctx, u8 dimensionality, u32 mip_levels)
	{
		// Select loader according to file extension
		if (ctx.path().extension() == ".exr")
		{
			// Load EXR images with TinyEXR
			return load_image_tinyexr(ctx, dimensionality, mip_levels);
		}
		else
		{
			// Load other image formats with stb_image
			return load_image_stb_image(ctx, dimensionality, mip_levels);
		}
	}

	template <>
	std::unique_ptr<gl::image_1d> resource_loader<gl::image_1d>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		return std::unique_ptr<gl::image_1d>(static_cast<gl::image_1d*>(load_image(*ctx, 1, 0).release()));
	}

	template <>
	std::unique_ptr<gl::image_2d> resource_loader<gl::image_2d>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		return std::unique_ptr<gl::image_2d>(static_cast<gl::image_2d*>(load_image(*ctx, 2, 0).release()));
	}

	template <>
	std::unique_ptr<gl::image_3d> resource_loader<gl::image_3d>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		return std::unique_ptr<gl::image_3d>(static_cast<gl::image_3d*>(load_image(*ctx, 3, 0).release()));
	}

	template <>
	std::unique_ptr<gl::image_cube> resource_loader<gl::image_cube>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		// Load cube map
		auto cube_map = std::unique_ptr<gl::image_2d>(static_cast<gl::image_2d*>(load_image(*ctx, 2, 1).release()));

		// Determine cube map layout
		const auto layout = gl::infer_cube_map_layout(cube_map->get_dimensions()[0], cube_map->get_dimensions()[1]);
		if (layout == gl::cube_map_layout::unknown)
		{
			throw deserialize_error("Failed to load cube image from cube map with unknown layout.");
		}
		else if (layout == gl::cube_map_layout::equirectangular || layout == gl::cube_map_layout::spherical)
		{
			throw deserialize_error("Failed to load cube image from cube map with unsupported layout.");
		}

		// Determine cube map face width
		const auto face_width = gl::infer_cube_map_face_width(cube_map->get_dimensions()[0], cube_map->get_dimensions()[1], layout);

		// Allocate cube image
		auto image = std::make_unique<gl::image_cube>
			(
				cube_map->get_format(),
				face_width,
				static_cast<u32>(std::bit_width(face_width))
			);

		// Vertical cross layout face offsets
		constexpr u32 vcross_offsets[6][2] =
		{
			{2, 2}, {0, 2}, // -x, +x
			{1, 3}, {1, 1}, // -y, +y
			{1, 0}, {1, 2}  // -z, +z
		};

		// Horizontal cross layout face offsets
		constexpr u32 hcross_offsets[6][2] =
		{
			{2, 1}, {0, 1}, // -x, +x
			{1, 2}, {1, 0}, // -y, +y
			{3, 1}, {1, 1}  // -z, +z
		};

		// Copy cube map faces to cube image
		switch (layout)
		{
			case gl::cube_map_layout::column:
				for (u32 i = 0; i < 6; ++i)
				{
					cube_map->copy(0, 0, face_width * i, 0, *image, 0, 0, 0, i, face_width, face_width, 1);
				}
				break;

			case gl::cube_map_layout::row:
				for (u32 i = 0; i < 6; ++i)
				{
					cube_map->copy(0, face_width * i, 0, 0, *image, 0, 0, 0, i, face_width, face_width, 1);
				}
				break;

			case gl::cube_map_layout::vertical_cross:
				for (u32 i = 0; i < 6; ++i)
				{
					cube_map->copy(0, face_width * vcross_offsets[i][0], face_width * vcross_offsets[i][1], 0, *image, 0, 0, 0, i, face_width, face_width, 1);
				}
				break;

			case gl::cube_map_layout::horizontal_cross:
				for (u32 i = 0; i < 6; ++i)
				{
					cube_map->copy(0, face_width * hcross_offsets[i][0], face_width * hcross_offsets[i][1], 0, *image, 0, 0, 0, i, face_width, face_width, 1);
				}
				break;

			case gl::cube_map_layout::equirectangular:
				[[fallthrough]];
			case gl::cube_map_layout::spherical:
				[[fallthrough]];
			case gl::cube_map_layout::unknown:
				[[fallthrough]];
			default:
				break;
		}

		// Generate mipmaps
		image->generate_mipmaps();

		return image;
	}
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/format.hpp>
#include <engine/utility/sized-types.hpp>
#include <cstddef>
#include <array>
#include <span>
#include <limits>
#include <utility>

namespace engine::gl
{
	/// Image flags.
	enum class image_flag: u8
	{
		/// Cube map view compatible image.
		cube_compatible = 0b00000001
	};

	/// Image base class.
	class image
	{
	public:
		/// Destructs an image.
		virtual ~image() = 0;

		image(const image&) = delete;
		image(image&&) = delete;
		image& operator=(const image&) = delete;
		image& operator=(image&&) = delete;

		/// Reads pixel data from the image.
		/// @param mip_level Level-of-detail number. Level `0` is the base image level. Level `n` is the nth mipmap reduction image.
		/// @param offset_x Texel offset in the X-direction.
		/// @param offset_y Texel offset in the Y-direction.
		/// @param offset_z Texel offset in the Z-direction.
		/// @param width Width of the subimage.
		/// @param height Height of the subimage.
		/// @param depth Depth of the subimage.
		/// @param format Format of the image data.
		/// @param data Buffer into which image data will be read.
		/// @exception std::out_of_range Image read operation mip level out of range.
		/// @exception std::invalid_argument Image read operation used unsupported format.
		void read
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
		) const;

		/// Writes pixel data to the image.
		/// @param mip_level Level-of-detail number. Level `0` is the base image level. Level `n` is the nth mipmap reduction image.
		/// @param offset_x Texel offset in the X-direction.
		/// @param offset_y Texel offset in the Y-direction.
		/// @param offset_z Texel offset in the Z-direction.
		/// @param width Width of the subimage.
		/// @param height Height of the subimage.
		/// @param depth Depth of the subimage.
		/// @param format Format of the image data.
		/// @param data Image data to write.
		/// @exception std::out_of_range Image write operation mip level out of range.
		/// @exception std::invalid_argument Image write operation used unsupported format.
		/// @exception std::out_of_range Image write operation exceeded image bounds.
		void write
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
		);

		/// Copies pixel data from this image into another the image.
		/// @param src_mip_level Source image level-of-detail number. Level `0` is the base image level. Level `n` is the nth mipmap reduction image.
		/// @param src_x Source image texel offset in the X-direction.
		/// @param src_y Source image texel offset in the Y-direction.
		/// @param src_z Source image texel offset in the Z-direction.
		/// @param dst_image Destination image.
		/// @param dst_mip_level Destination image level-of-detail number. Level `0` is the base image level. Level `n` is the nth mipmap reduction image.
		/// @param dst_x Destination image texel offset in the X-direction.
		/// @param dst_y Destination image texel offset in the Y-direction.
		/// @param dst_z Destination image texel offset in the Z-direction.
		/// @param width Width of the subimage to copy.
		/// @param height Height of the subimage to copy.
		/// @param depth Depth of the subimage to copy.
		void copy
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
		) const;

		/// Generates mip subimages.
		void generate_mipmaps();

		/// Returns the dimensionality of the image.
		[[nodiscard]] inline constexpr u8 get_dimensionality() const noexcept
		{
			return m_dimensionality;
		}

		/// Returns `true` if the image is 1D, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_1d() const noexcept
		{
			return m_dimensionality == 1;
		}

		/// Returns `true` if the image is 2D, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_2d() const noexcept
		{
			return m_dimensionality == 2;
		}

		/// Returns `true` if the image is 3D, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_3d() const noexcept
		{
			return m_dimensionality == 3;
		}

		/// Returns the format and type of the texel blocks contained in the image.
		[[nodiscard]] inline constexpr format get_format() const noexcept
		{
			return m_format;
		}

		/// Returns the dimensions of the image.
		[[nodiscard]] inline constexpr const std::array<u32, 3>& get_dimensions() const noexcept
		{
			return m_dimensions;
		}

		/// Returns the number of levels of detail available for minified sampling of the image.
		[[nodiscard]] inline constexpr u32 get_mip_levels() const noexcept
		{
			return m_mip_levels;
		}

		/// Returns the number of layers in the image.
		[[nodiscard]] inline constexpr u32 get_array_layers() const noexcept
		{
			return m_array_layers;
		}

		/// Returns the image flags.
		[[nodiscard]] inline constexpr u32 get_flags() const noexcept
		{
			return m_flags;
		}

		/// Returns `true` if the image is cube map compatible, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_cube_compatible() const noexcept
		{
			return m_flags & std::to_underlying(image_flag::cube_compatible);
		}

	protected:
		/// Constructs an image.
		/// @param dimensionality Image dimensionality, on `[1, 3]`.
		/// @param format Format and type of the texel blocks that will be contained in the image.
		/// @param width Width of the image.
		/// @param height Height of the image.
		/// @param depth Depth of the image.
		/// @param mip_levels Number of levels of detail available for minified sampling of the image.
		/// @param array_layers Number of layers in the image.
		/// @param flags Image flags.
		/// @exception std::invalid_argument Image constructed with unsupported format.
		/// @exception std::invalid_argument Image dimensions must be nonzero.
		/// @exception std::invalid_argument Image mip levels must be nonzero.
		/// @exception std::out_of_range Image mip levels exceed `1 + log2(max(width, height, depth))`.
		/// @exception std::invalid_argument Image array layers must be nonzero.
		/// @exception std::invalid_argument 1D image must have a height and depth of `1`.
		/// @exception std::invalid_argument 2D image must have a depth of `1`.
		/// @exception std::invalid_argument 3D image arrays not supported.
		/// @exception std::invalid_argument Cube compatible image must be 2D.
		/// @exception std::invalid_argument Cube compatible image width and height must be equal.
		/// @exception std::invalid_argument Cube compatible image array layers must be a multiple of 6.
		image
		(
			u8 dimensionality,
			gl::format format,
			u32 width,
			u32 height,
			u32 depth,
			u32 mip_levels,
			u32 array_layers,
			u32 flags
		);

	private:
		unsigned int m_gl_texture_target{0};
		unsigned int m_gl_texture_name{0};
		u8 m_dimensionality{0};
		format m_format{format::undefined};
		std::array<u32, 3> m_dimensions{0, 0, 0};
		u32 m_mip_levels{0};
		u32 m_array_layers{0};
		u32 m_flags{0};

		friend class image_view;
	};

	/// 1D image.
	class image_1d: public image
	{
	public:
		/// @copydoc image::image
		image_1d
		(
			gl::format format,
			u32 width,
			u32 mip_levels = 1,
			u32 array_layers = 1,
			u32 flags = 0
		);

		/// Destructs a 1D image.
		~image_1d() override = default;
	};

	/// 2D image.
	class image_2d: public image
	{
	public:
		/// @copydoc image::image
		image_2d
		(
			gl::format format,
			u32 width,
			u32 height,
			u32 mip_levels = 1,
			u32 array_layers = 1,
			u32 flags = 0
		);

		/// Destructs a 2D image.
		~image_2d() override = default;
	};

	/// 3D image.
	class image_3d: public image
	{
	public:
		/// @copydoc image::image
		image_3d
		(
			gl::format format,
			u32 width,
			u32 height,
			u32 depth,
			u32 mip_levels = 1,
			u32 flags = 0
		);

		/// Destructs a 3D image.
		~image_3d() override = default;
	};

	/// Cube-compatible 2D image.
	class image_cube: public image_2d
	{
	public:
		/// @copydoc image_2d::image_2d
		image_cube
		(
			gl::format format,
			u32 width,
			u32 mip_levels = 1,
			u32 array_layers = 6
		);

		/// Destructs a cube image.
		~image_cube() override = default;
	};
}

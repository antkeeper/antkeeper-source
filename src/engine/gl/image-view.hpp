// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/image.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>
#include <utility>

namespace engine::gl::opengl
{
	class shader_texture_1d;
	class shader_texture_2d;
	class shader_texture_3d;
	class shader_texture_cube;
}

namespace engine::gl
{
	class framebuffer;

	/// Image view flags.
	enum class image_view_flag: u8
	{
		/// Image array view.
		array = 0b00000001,

		/// Cube map view.
		cube = 0b00000010
	};

	/// Image view base class.
	class image_view
	{
	public:
		/// Destructs an image view.
		virtual ~image_view() = 0;

		image_view(const image_view&) = delete;
		image_view(image_view&&) = delete;
		image_view& operator=(const image_view&) = delete;
		image_view& operator=(image_view&&) = delete;

		/// Returns the image on which the view was created.
		[[nodiscard]] inline constexpr const std::shared_ptr<image>& get_image() const noexcept
		{
			return m_image;
		}

		/// Returns the format and type used to interpret texel blocks of the image.
		[[nodiscard]] inline constexpr format get_format() const noexcept
		{
			return m_format;
		}

		/// Returns the first mipmap level accessible to the view.
		[[nodiscard]] inline constexpr u32 get_first_mip_level() const noexcept
		{
			return m_first_mip_level;
		}

		/// Returns the number of mipmap levels accessible to the view.
		[[nodiscard]] inline constexpr u32 get_mip_level_count() const noexcept
		{
			return m_mip_level_count;
		}

		/// Returns the first array layer accessible to the view.
		[[nodiscard]] inline constexpr u32 get_first_array_layer() const noexcept
		{
			return m_first_array_layer;
		}

		/// Returns the number of array layers accessible to the view.
		[[nodiscard]] inline constexpr u32 get_array_layer_count() const noexcept
		{
			return m_array_layer_count;
		}

		/// Returns the dimensionality of the image view.
		[[nodiscard]] inline constexpr u8 get_dimensionality() const noexcept
		{
			return m_dimensionality;
		}

		/// Returns `true` if the image view is 1D, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_1d() const noexcept
		{
			return m_dimensionality == 1;
		}

		/// Returns `true` if the image view is 2D, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_2d() const noexcept
		{
			return m_dimensionality == 2;
		}

		/// Returns `true` if the image view is 3D, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_3d() const noexcept
		{
			return m_dimensionality == 3;
		}

		/// Returns `true` if the image view is an array view, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_array() const noexcept
		{
			return m_flags & std::to_underlying(image_view_flag::array);
		}

		/// Returns `true` if the image view is a cube map view, `false` otherwise.
		[[nodiscard]] inline constexpr bool is_cube() const noexcept
		{
			return m_flags & std::to_underlying(image_view_flag::cube);
		}

	protected:
		/// Constructs an image view from an image.
		/// @param image Image on which the view will be created.
		/// @param dimensionality Image view dimensionality, on `[1, 3]`.
		/// @param format Format and type used to interpret texel blocks of the image. If gl::format::undefined, the format will be set to the format of the image.
		/// @param first_mip_level First mipmap level accessible to the view.
		/// @param mip_level_count Number of mipmap levels accessible to the view.
		/// @param first_array_layer First array layer accessible to the view.
		/// @param array_layer_count Number of array layers accessible to the view.
		/// @param flags Image view flags.
		/// @exception std::invalid_argument Image view has null image.
		/// @exception std::invalid_argument Image view has unsupported format.
		/// @exception std::invalid_argument Image view has zero mip levels.
		/// @exception std::out_of_range Image view mip range out of image mip range.
		/// @exception std::invalid_argument Image view has zero array layers.
		/// @exception std::out_of_range Image view array layer range out of image array layer range.
		/// @exception std::invalid_argument Image view dimensionality must match image dimensionality.
		/// @exception std::invalid_argument Cube image views must be constructed from cube-compatible images.
		/// @exception std::invalid_argument Cube image views array layer count must be a multiple of 6.
		image_view
		(
			std::shared_ptr<gl::image> image,
			u8 dimensionality,
			gl::format format,
			u32 first_mip_level,
			u32 mip_level_count,
			u32 first_array_layer,
			u32 array_layer_count,
			u8 flags
		);

	private:
		friend class framebuffer;
		friend class opengl::shader_texture_1d;
		friend class opengl::shader_texture_2d;
		friend class opengl::shader_texture_3d;
		friend class opengl::shader_texture_cube;

		unsigned int m_gl_texture_name{0};
		std::shared_ptr<image> m_image;
		u8 m_dimensionality{0};
		format m_format{format::undefined};
		u32 m_first_mip_level{0};
		u32 m_mip_level_count{0};
		u32 m_first_array_layer{0};
		u32 m_array_layer_count{0};
		u8 m_flags{0};
	};

	/// 1D image view.
	class image_view_1d: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_1d
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1,
			u32 first_array_layer = 0
		);

		/// Destructs a 1D image view.
		~image_view_1d() override = default;
	};

	/// 1D image array view.
	class image_view_1d_array: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_1d_array
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1,
			u32 first_array_layer = 0,
			u32 array_layer_count = 1
		);

		/// Destructs a 1D image array view.
		~image_view_1d_array() override = default;
	};

	/// 2D image view.
	class image_view_2d: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_2d
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1,
			u32 first_array_layer = 0
		);

		/// Destructs a 2D image view.
		~image_view_2d() override = default;
	};

	/// 2D image array view.
	class image_view_2d_array: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_2d_array
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1,
			u32 first_array_layer = 0,
			u32 array_layer_count = 1
		);

		/// Destructs a 2D image array view.
		~image_view_2d_array() override = default;
	};

	/// 3D image view.
	class image_view_3d: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_3d
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1
		);

		/// Destructs a 3D image view.
		~image_view_3d() override = default;
	};

	/// Cube image view.
	class image_view_cube: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_cube
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1,
			u32 first_array_layer = 0
		);

		/// Destructs a cube image view.
		~image_view_cube() override = default;
	};

	/// Cube image array view.
	class image_view_cube_array: public image_view
	{
	public:
		/// @copydoc image_view::image_view
		explicit image_view_cube_array
		(
			std::shared_ptr<gl::image> image,
			gl::format format = gl::format::undefined,
			u32 first_mip_level = 0,
			u32 mip_level_count = 1,
			u32 first_array_layer = 0,
			u32 array_layer_count = 6
		);

		/// Destructs a cube image array view.
		~image_view_cube_array() override = default;
	};
}

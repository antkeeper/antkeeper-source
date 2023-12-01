// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GL_IMAGE_VIEW_HPP
#define ANTKEEPER_GL_IMAGE_VIEW_HPP

#include <engine/gl/image.hpp>
#include <engine/gl/image-view-flag.hpp>
#include <cstdint>
#include <memory>

namespace gl {

/**
 * Image view.
 */
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
	[[nodiscard]] inline constexpr std::uint32_t get_first_mip_level() const noexcept
	{
		return m_first_mip_level;
	}
	
	/// Returns the number of mipmap levels accessible to the view.
	[[nodiscard]] inline constexpr std::uint32_t get_mip_level_count() const noexcept
	{
		return m_mip_level_count;
	}
	
	/// Returns the first array layer accessible to the view.
	[[nodiscard]] inline constexpr std::uint32_t get_first_array_layer() const noexcept
	{
		return m_first_array_layer;
	}
	
	/// Returns the number of array layers accessible to the view.
	[[nodiscard]] inline constexpr std::uint32_t get_array_layer_count() const noexcept
	{
		return m_array_layer_count;
	}
	
	/// Returns the dimensionality of the image view.
	[[nodiscard]] inline constexpr std::uint8_t get_dimensionality() const noexcept
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
	/**
	 * Constructs an image view from an image.
	 *
	 * @param image Image on which the view will be created.
	 * @param dimensionality Image view dimensionality, on `[1, 3]`.
	 * @param format Format and type used to interpret texel blocks of the image. If gl::format::undefined, the format will be set to the format of the image.
	 * @param first_mip_level First mipmap level accessible to the view.
	 * @param mip_level_count Number of mipmap levels accessible to the view.
	 * @param first_array_layer First array layer accessible to the view.
	 * @param array_layer Number of array layers accessible to the view.
	 * @param flags Image view flags.
	 *
	 * @except std::invalid_argument Image view has null image.
	 * @except std::invalid_argument Image view has unsupported format.
	 * @except std::invalid_argument Image view has zero mip levels.
	 * @except std::out_of_range Image view mip range out of image mip range.
	 * @except std::invalid_argument Image view has zero array layers.
	 * @except std::out_of_range Image view array layer range out of image array layer range.
	 * @except std::invalid_argument Image view dimensionality must match image dimensionality.
	 * @except std::invalid_argument Cube image views must be constructed from cube-compatible images.
	 * @except std::invalid_argument Cube image views array layer count must be a multiple of 6.
	 */
	image_view
	(
		std::shared_ptr<gl::image> image,
		std::uint8_t dimensionality,
		gl::format format,
		std::uint32_t first_mip_level,
		std::uint32_t mip_level_count,
		std::uint32_t first_array_layer,
		std::uint32_t array_layer_count,
		std::uint8_t flags
	);
	
private:
	friend class framebuffer;
	friend class gl_shader_texture_1d;
	friend class gl_shader_texture_2d;
	friend class gl_shader_texture_3d;
	friend class gl_shader_texture_cube;
	
	unsigned int m_gl_texture_name{0};
	std::shared_ptr<image> m_image;
	std::uint8_t m_dimensionality{0};
	format m_format{format::undefined};
	std::uint32_t m_first_mip_level{0};
	std::uint32_t m_mip_level_count{0};
	std::uint32_t m_first_array_layer{0};
	std::uint32_t m_array_layer_count{0};
	std::uint8_t m_flags{0};
};

/**
 * 1D image view.
 */
class image_view_1d: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_1d
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1,
		std::uint32_t first_array_layer = 0
	);
};

/**
 * 1D image array view.
 */
class image_view_1d_array: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_1d_array
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1,
		std::uint32_t first_array_layer = 0,
		std::uint32_t array_layer_count = 1
	);
};

/**
 * 2D image view.
 */
class image_view_2d: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_2d
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1,
		std::uint32_t first_array_layer = 0
	);
};

/**
 * 2D image array view.
 */
class image_view_2d_array: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_2d_array
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1,
		std::uint32_t first_array_layer = 0,
		std::uint32_t array_layer_count = 1
	);
};

/**
 * 3D image view.
 */
class image_view_3d: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_3d
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1
	);
};

/**
 * Cube image view.
 */
class image_view_cube: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_cube
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1,
		std::uint32_t first_array_layer = 0
	);
};

/**
 * Cube image array view.
 */
class image_view_cube_array: public image_view
{
public:
	/// @copydoc image_view::image_view
	explicit image_view_cube_array
	(
		std::shared_ptr<gl::image> image,
		gl::format format = gl::format::undefined,
		std::uint32_t first_mip_level = 0,
		std::uint32_t mip_level_count = 1,
		std::uint32_t first_array_layer = 0,
		std::uint32_t array_layer_count = 6
	);
};

} // namespace gl

#endif // ANTKEEPER_GL_IMAGE_VIEW_HPP

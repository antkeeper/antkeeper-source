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

#ifndef ANTKEEPER_GL_TEXTURE_HPP
#define ANTKEEPER_GL_TEXTURE_HPP

#include <engine/gl/color-space.hpp>
#include <engine/gl/pixel-format.hpp>
#include <engine/gl/pixel-type.hpp>
#include <engine/gl/texture-filter.hpp>
#include <engine/gl/texture-type.hpp>
#include <engine/gl/texture-wrapping.hpp>
#include <array>
#include <cstdint>
#include <cstddef>
#include <span>
#include <tuple>

namespace gl {

class framebuffer;
class gl_shader_texture_1d;
class gl_shader_texture_2d;
class gl_shader_texture_3d;
class gl_shader_texture_cube;

/**
 * Abstract base class for 1D, 2D, 3D, and cube textures which can be uploaded to shaders via shader inputs.
 */
class texture
{
public:
	/**
	 * Destructs a texture.
	 */
	virtual ~texture();
	
	/**
	 * Reads texture pixel data from the GPU.
	 *
	 * @param[out] data Pixel data buffer.
	 * @param[in] type Returned pixel component data type.
	 * @param[in] format Returned pixel format.
	 * @param[in] level Mip level to read.
	 */
	void read(std::span<std::byte> data, gl::pixel_type type, gl::pixel_format format, std::uint8_t level = 0) const;
	
	/**
	 * Sets the texture filter modes.
	 *
	 * @param min_filter Texture minification filter mode.
	 * @param mag_filter Texture magnification filter mode.
	 */
	void set_filters(texture_min_filter min_filter, texture_mag_filter mag_filter);
	
	/**
	 * Sets the texture minification filter mode.
	 *
	 * @param filter Texture minification filter mode.
	 */
	void set_min_filter(texture_min_filter filter);
	
	/**
	 * Sets the texture magnification filter mode.
	 *
	 * @param filter Texture magnification filter mode.
	 */
	void set_mag_filter(texture_mag_filter filter);
	
	/**
	 * Sets the index of lowest accessible mip level.
	 *
	 * @param level Index of the lowest accessible mip level.
	 */
	void set_base_level(std::uint8_t level);
	
	/**
	 * Sets the index of highest accessible mip level.
	 *
	 * @param level Index of the highest accessible mip level.
	 */
	void set_max_level(std::uint8_t level);
	
	/**
	 * Sets the range of accessible mip levels.
	 *
	 * @param base Index of the lowest accessible mip level.
	 * @param max Index of the highest accessible mip level.
	 */
	void set_mip_range(std::uint8_t base_level, std::uint8_t max_level);
	
	/**
	 * Sets the maximum anisotropy.
	 *
	 * @param level Max anisotropy on `[0.0, 1.0]`, with `0.0` indicating normal filtering, and `1.0` indicating maximum anisotropic filtering.
	 */
	void set_max_anisotropy(float anisotropy);
	
	/// Returns the texture type.
	[[nodiscard]] virtual constexpr texture_type get_texture_type() const noexcept = 0;
	
	/// Returns the dimensions of the texture, in pixels.
	[[nodiscard]] inline const std::array<std::uint16_t, 3>& get_dimensions() const noexcept
	{
		return m_dimensions;
	}
	
	/// Returns the width of the texture, in pixels.
	[[nodiscard]] inline std::uint16_t get_width() const noexcept
	{
		return m_dimensions[0];
	}
	
	/// Returns the height of the texture, in pixels.
	[[nodiscard]] inline std::uint16_t get_height() const noexcept
	{
		return m_dimensions[1];
	}
	
	/// Returns the depth of the texture, in pixels.
	[[nodiscard]] inline std::uint16_t get_depth() const noexcept
	{
		return m_dimensions[2];
	}
	
	/// Returns the pixel type enumeration.
	[[nodiscard]] inline pixel_type get_pixel_type() const noexcept
	{
		return m_pixel_type;
	}
	
	/// Returns the pixel format enumeration.
	[[nodiscard]] inline pixel_format get_pixel_format() const noexcept
	{
		return m_pixel_format;
	}
	
	/// Returns the color space enumeration.
	[[nodiscard]] inline color_space get_color_space() const noexcept
	{
		return m_color_space;
	}
	
	/// Returns the wrapping modes of the texture.
	[[nodiscard]] inline const std::array<texture_wrapping, 3>& get_wrapping() const noexcept
	{
		return m_wrapping;
	}
	
	/// Returns the filtering modes of the texture.
	[[nodiscard]] inline const std::tuple<texture_min_filter, texture_mag_filter>& get_filters() const noexcept
	{
		return m_filters;
	}
	
	/// Returns the number of available mip levels.
	[[nodiscard]] inline std::uint16_t get_mip_count() const noexcept
	{
		return m_mip_count;
	}
	
	/// Returns the index of the lowest accessible mip level.
	[[nodiscard]] inline std::uint8_t get_base_level() const noexcept
	{
		return m_base_level;
	}
	
	/// Returns the index of the highest accessible mip level.
	[[nodiscard]] inline std::uint8_t get_max_level() const noexcept
	{
		return m_max_level;
	}
	
	/// Returns the maximum anisotropy.
	[[nodiscard]] inline float get_max_anisotropy() const noexcept
	{
		return m_max_anisotropy;
	}
	
protected:
	/**
	 * Constructs a texture.
	 *
	 * @param width Texture width, in pixels.
	 * @param height Texture height, in pixels. For 2D or 3D textures.
	 * @param depth Texture depth, in pixels. For 3D textures only.
	 * @param type Pixel component data type.
	 * @param format Pixel format.
	 * @param color_space Color space of the pixel data.
	 * @param data Pointer to pixel data.
	 *
	 * @warning If the sRGB color space is specified, pixel data will be stored internally as 8 bits per channel, and automatically converted to linear space before reading.
	 */
	/// @{
	texture(std::uint16_t width, std::uint16_t height, std::uint16_t depth, bool cube = false, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	texture(std::uint16_t width, std::uint16_t height, bool cube = false, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	explicit texture(std::uint16_t width, bool cube = false, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	/// @}
	
	/**
	 * Sets the texture wrapping modes.
	 *
	 * @param wrap_s Wrapping mode for s-coordinates.
	 * @param wrap_t Wrapping mode for t-coordinates.
	 * @param wrap_r Wrapping mode for r-coordinates.
	 */
	/// @{
	virtual void set_wrapping(gl::texture_wrapping wrap_s, gl::texture_wrapping wrap_t, gl::texture_wrapping wrap_r);
	virtual void set_wrapping(gl::texture_wrapping wrap_s, gl::texture_wrapping wrap_t);
	virtual void set_wrapping(gl::texture_wrapping wrap_s);
	/// @}
	
	/**
	 * Resizes the texture.
	 *
	 * @param width Texture width, in pixels.
	 * @param height Texture height, in pixels. For 2D or 3D textures.
	 * @param depth Texture depth, in pixels. For 3D textures only.
	 * @param type Pixel component data type.
	 * @param format Pixel format.
	 * @param color_space Color space of the pixel data.
	 * @param data Pointer to pixel data.
	 *
	 * @warning If the sRGB color space is specified, pixel data will be stored internally as 8 bits per channel, and automatically converted to linear space before reading.
	 */
	/// @{
	virtual void resize(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data);
	virtual void resize(std::uint16_t width, std::uint16_t height, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data);
	virtual void resize(std::uint16_t width, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const std::byte* data);
	/// @}

private:
	void update_cube_faces(unsigned int gl_internal_format, unsigned int gl_format, unsigned int gl_type, const std::byte* data);
	
	friend class framebuffer;
	friend class gl_shader_texture_1d;
	friend class gl_shader_texture_2d;
	friend class gl_shader_texture_3d;
	friend class gl_shader_texture_cube;
	
	unsigned int m_gl_texture_target{};
	unsigned int m_gl_texture_id{};
	std::array<std::uint16_t, 3> m_dimensions{};
	gl::pixel_type m_pixel_type{};
	gl::pixel_format m_pixel_format{};
	gl::color_space m_color_space{};
	std::array<texture_wrapping, 3> m_wrapping{texture_wrapping::repeat, texture_wrapping::repeat, texture_wrapping::repeat};
	std::tuple<texture_min_filter, texture_mag_filter> m_filters{texture_min_filter::linear_mipmap_linear, texture_mag_filter::linear};
	std::uint8_t m_base_level{};
	std::uint8_t m_max_level{255};
	float m_max_anisotropy{};
	
protected:
	std::uint16_t m_mip_count{};
};

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_HPP

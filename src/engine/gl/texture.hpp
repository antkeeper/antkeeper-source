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
#include <engine/gl/texture-wrapping.hpp>
#include <array>
#include <cstdint>
#include <cstddef>
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
	texture(std::uint16_t width, std::uint16_t height, std::uint16_t depth, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	texture(std::uint16_t width, std::uint16_t height, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	texture(std::uint16_t width, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const std::byte* data = nullptr);
	/// @}
	
	/**
	 * Destructs a texture.
	 */
	virtual ~texture() = 0;
	
	/**
	 * Sets the texture filtering modes.
	 *
	 * @param min_filter Texture minification filter mode.
	 * @param mag_filter Texture magnification filter mode.
	 */
	void set_filters(texture_min_filter min_filter, texture_mag_filter mag_filter);
	
	/**
	 * Sets the maximum anisotropy.
	 *
	 * @param level Max anisotropy on `[0.0, 1.0]`, with `0.0` indicating normal filtering, and `1.0` indicating maximum anisotropic filtering.
	 */
	void set_max_anisotropy(float anisotropy);
	
	/// Returns the dimensions of the texture, in pixels.
	const std::array<std::uint16_t, 3>& get_dimensions() const;
	
	/// Returns the width of the texture, in pixels.
	const std::uint16_t& get_width() const;
	
	/// Returns the height of the texture, in pixels.
	const std::uint16_t& get_height() const;
	
	/// Returns the depth of the texture, in pixels.
	const std::uint16_t& get_depth() const;
	
	/// Returns the pixel type enumeration.
	const pixel_type& get_pixel_type() const;
	
	/// Returns the pixel format enumeration.
	const pixel_format& get_pixel_format() const;
	
	/// Returns the color space enumeration.
	const color_space& get_color_space() const;
	
	/// Returns the wrapping modes of the texture.
	const std::array<texture_wrapping, 3>& get_wrapping() const;
	
	/// Returns the filtering modes of the texture.
	const std::tuple<texture_min_filter, texture_mag_filter>& get_filters() const;
	
	/// Returns the maximum anisotropy.
	float get_max_anisotropy() const;
	
protected:
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
	friend class framebuffer;
	friend class gl_shader_texture_1d;
	friend class gl_shader_texture_2d;
	friend class gl_shader_texture_3d;
	friend class gl_shader_texture_cube;
	
	unsigned int gl_texture_target{0};
	unsigned int gl_texture_id{0};
	std::array<std::uint16_t, 3> dimensions{0, 0, 0};
	gl::pixel_type pixel_type{0};
	gl::pixel_format pixel_format{0};
	gl::color_space color_space{0};
	std::array<texture_wrapping, 3> wrapping;
	std::tuple<texture_min_filter, texture_mag_filter> filters;
	float max_anisotropy{0.0f};
};

inline const std::array<std::uint16_t, 3>& texture::get_dimensions() const
{
	return dimensions;
}

inline const std::uint16_t& texture::get_width() const
{
	return dimensions[0];
}

inline const std::uint16_t& texture::get_height() const
{
	return dimensions[1];
}

inline const std::uint16_t& texture::get_depth() const
{
	return dimensions[2];
}

inline const pixel_type& texture::get_pixel_type() const
{
	return pixel_type;
}

inline const pixel_format& texture::get_pixel_format() const
{
	return pixel_format;
}

inline const color_space& texture::get_color_space() const
{
	return color_space;
}

inline const std::array<texture_wrapping, 3>& texture::get_wrapping() const
{
	return wrapping;
}

inline const std::tuple<texture_min_filter, texture_mag_filter>& texture::get_filters() const
{
	return filters;
}

inline float texture::get_max_anisotropy() const
{
	return max_anisotropy;
}

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_HPP

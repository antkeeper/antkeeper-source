/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_GL_TEXTURE_2D_HPP
#define ANTKEEPER_GL_TEXTURE_2D_HPP

#include <array>
#include <tuple>
#include "gl/color-space.hpp"
#include "gl/pixel-format.hpp"
#include "gl/pixel-type.hpp"

namespace gl {

class framebuffer;
class shader_input;
enum class texture_mag_filter;
enum class texture_min_filter;
enum class texture_wrapping;

/**
 * A 2D texture which can be uploaded to shaders via shader inputs.
 */
class texture_2d
{
public:
	/**
	 * Creates a 2D texture.
	 *
	 * @param color_space Specifies the color space of the pixel data.
	 *
	 * @warning If the sRGB color space is specified, pixel data will be stored internally as 8 bits per channel, and automatically converted to linear space before reading.
	 */
	texture_2d(int width, int height, gl::pixel_type type = gl::pixel_type::uint_8, gl::pixel_format format = gl::pixel_format::rgba, gl::color_space color_space = gl::color_space::linear, const void* data = nullptr);
	
	/**
	 * Destroys a 2D texture.
	 */
	~texture_2d();

	/**
	 * Resizes the texture.
	 *
	 * @warning If the sRGB color space is specified, pixel data will be stored internally as 8 bits per channel, and automatically converted to linear space before reading.
	 */
	void resize(int width, int height, gl::pixel_type type, gl::pixel_format format, gl::color_space color_space, const void* data);

	/**
	 * Sets the texture wrapping modes.
	 *
	 * @param wrap_s Wrapping mode for s-coordinates.
	 * @param wrap_t Wrapping mode for t-coordinates.
	 */
	void set_wrapping(gl::texture_wrapping wrap_s, texture_wrapping wrap_t);

	/**
	 * Sets the texture filtering modes.
	 *
	 * @param min_filter Texture minification filter.
	 * @param mag_filter Texture magnification filter.
	 */
	void set_filters(texture_min_filter min_filter, texture_mag_filter mag_filter);

	/**
	 * Sets the maximum anisotropy.
	 *
	 * @param level Max anisotropy on `[0.0, 1.0]`, with `0.0` indicating normal filtering, and `1.0` indicating maximum anisotropic filtering.
	 */
	void set_max_anisotropy(float anisotropy);

	/// Returns the dimensions of the texture, in pixels.
	const std::array<int, 2>& get_dimensions() const;
	
	/// Returns the pixel type enumeration.
	const pixel_type& get_pixel_type() const;
	
	/// Returns the pixel format enumeration.
	const pixel_format& get_pixel_format() const;
	
	/// Returns the color space enumeration.
	const color_space& get_color_space() const;

	/// Returns the wrapping modes of the texture.
	const std::tuple<texture_wrapping, texture_wrapping> get_wrapping() const;

	/// Returns the filtering modes of the texture.
	const std::tuple<texture_min_filter, texture_mag_filter> get_filters() const;

	/// Returns the maximum anisotropy.
	float get_max_anisotropy() const;

private:
	friend class framebuffer;
	friend class shader_input;

	unsigned int gl_texture_id;
	std::array<int, 2> dimensions;
	gl::pixel_type pixel_type;
	gl::pixel_format pixel_format;
	gl::color_space color_space;
	std::tuple<texture_wrapping, texture_wrapping> wrapping;
	std::tuple<texture_min_filter, texture_mag_filter> filters;
	float max_anisotropy;
};

inline const std::array<int, 2>& texture_2d::get_dimensions() const
{
	return dimensions;
}

inline const pixel_type& texture_2d::get_pixel_type() const
{
	return pixel_type;
}

inline const pixel_format& texture_2d::get_pixel_format() const
{
	return pixel_format;
}

inline const color_space& texture_2d::get_color_space() const
{
	return color_space;
}

inline const std::tuple<texture_wrapping, texture_wrapping> texture_2d::get_wrapping() const
{
	return wrapping;
}

inline const std::tuple<texture_min_filter, texture_mag_filter> texture_2d::get_filters() const
{
	return filters;
}

inline float texture_2d::get_max_anisotropy() const
{
	return max_anisotropy;
}

} // namespace gl

#endif // ANTKEEPER_GL_TEXTURE_2D_HPP


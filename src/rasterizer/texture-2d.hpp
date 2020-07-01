/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_TEXTURE_2D_HPP
#define ANTKEEPER_TEXTURE_2D_HPP

#include <tuple>
#include "rasterizer/pixel-format.hpp"
#include "rasterizer/pixel-type.hpp"

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
	 */
	texture_2d(int width, int height, ::pixel_type type = ::pixel_type::uint_8, ::pixel_format format = ::pixel_format::rgba, const void* data = nullptr);
	
	/**
	 * Destroys a 2D texture.
	 */
	~texture_2d();

	/**
	 * Resizes the texture.
	 */
	void resize(int width, int height, ::pixel_type type, ::pixel_format format, const void* data);

	/**
	 * Sets the texture wrapping modes.
	 *
	 * @param wrap_s Wrapping mode for s-coordinates.
	 * @param wrap_t Wrapping mode for t-coordinates.
	 */
	void set_wrapping(texture_wrapping wrap_s, texture_wrapping wrap_t);

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
	const std::tuple<int, int>& get_dimensions() const;
	
	/// Returns the pixel type enumeration.
	const pixel_type& get_pixel_type() const;
	
	/// Returns the pixel format enumeration.
	const pixel_format& get_pixel_format() const;

	/// Returns the wrapping modes of the texture.
	const std::tuple<texture_wrapping, texture_wrapping> get_wrapping() const;

	/// Returns the filtering modes of the texture.
	const std::tuple<texture_min_filter, texture_mag_filter> get_filters() const;

	/// Returs the maximum anisotropy.
	float get_max_anisotropy() const;

private:
	friend class framebuffer;
	friend class shader_input;

	unsigned int gl_texture_id;
	std::tuple<int, int> dimensions;
	::pixel_type pixel_type;
	::pixel_format pixel_format;
	std::tuple<texture_wrapping, texture_wrapping> wrapping;
	std::tuple<texture_min_filter, texture_mag_filter> filters;
	float max_anisotropy;
};

inline const std::tuple<int, int>& texture_2d::get_dimensions() const
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

#endif // ANTKEEPER_TEXTURE_2D_HPP


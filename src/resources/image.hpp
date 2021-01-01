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

#ifndef ANTKEEPER_IMAGE_HPP
#define ANTKEEPER_IMAGE_HPP

#include <cstdlib>

/**
 * Stores basic image data.
 */
class image
{
public:
	/// Creates an image.
	image();

	/// Destroys an image.
	~image();

	/**
	 * Changes the format of the image. Existing pixel data will be erased if the format has changed.
	 *
	 * @param channels Number of color channels.
	 * @param hdr Whether or not the image will contain HDR data. HDR pixels are stored as floats. Standard pixels are stored as unsigned chars.
	 */
	void format(unsigned int channels, bool hdr);

	/**
	 * Resizes the image. Existing pixel data will be erased if the size has changed.
	 *
	 * @param width New width of the image, in pixels.
	 * @param height New height of the image, in pixels.
	 */
	void resize(unsigned int width, unsigned int height);

	/// Returns whether or not the image contains HDR data.
	bool is_hdr() const;

	/// Returns the width of the image, in pixels.
	unsigned int get_width() const;

	/// Returns the height of the image, in pixels.
	unsigned int get_height() const;

	/// Returns the number of color channels in the image. 
	unsigned int get_channels() const;

	/// Returns a pointer to the pixel data.
	const void* get_pixels() const;
	
	/// @copydoc image::get_pixels() const
	void* get_pixels();
	
	/// Returns the size of the image, in bytes.
	std::size_t get_size() const;

private:
	void allocate_pixels();
	void free_pixels();

	bool hdr;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
	void* pixels;
	std::size_t size;
};

inline bool image::is_hdr() const
{
	return hdr;
}

inline unsigned int image::get_width() const
{
	return width;
}

inline unsigned int image::get_height() const
{
	return height;
}

inline unsigned int image::get_channels() const
{
	return channels;
}

inline const void* image::get_pixels() const
{
	return pixels;
}

inline void* image::get_pixels()
{
	return pixels;
}

inline std::size_t image::get_size() const
{
	return size;
}

#endif // ANTKEEPER_IMAGE_HPP

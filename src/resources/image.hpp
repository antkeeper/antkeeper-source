/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IMAGE_HPP
#define IMAGE_HPP

/**
 * Stores basic image data.
 */
class Image
{
public:
	/// Creates an image.
	Image();

	/// Destroys an image.
	~Image();

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
	bool isHDR() const;

	/// Returns the width of the image, in pixels.
	unsigned int getWidth() const;

	/// Returns the height of the image, in pixels.
	unsigned int getHeight() const;

	/// Returns the number of color channels in the image. 
	unsigned int getChannels() const;

	/// Returns a pointer to the pixel data.
	const void* getPixels() const;
	
	/// @copydoc Image::getPixels() const
	void* getPixels();

private:
	void allocatePixels();
	void freePixels();

	bool hdr;
	unsigned int width;
	unsigned int height;
	unsigned int channels;
	void* pixels;
};

inline bool Image::isHDR() const
{
	return hdr;
}

inline unsigned int Image::getWidth() const
{
	return width;
}

inline unsigned int Image::getHeight() const
{
	return height;
}

inline unsigned int Image::getChannels() const
{
	return channels;
}

inline const void* Image::getPixels() const
{
	return pixels;
}

inline void* Image::getPixels()
{
	return pixels;
}

#endif // IMAGE_HPP


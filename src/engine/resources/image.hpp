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

#ifndef ANTKEEPER_IMAGE_HPP
#define ANTKEEPER_IMAGE_HPP

#include <engine/math/vector.hpp>
#include <cstddef>
#include <type_traits>

/**
 * Stores basic image data.
 */
class image
{
public:
	/**
	 * Creates a copy of another image.
	 *
	 * @param source Image from which to copy.
	 */
	image(const image& source);
	
	/// Creates an image.
	image();

	/// Destroys an image.
	~image();
	
	/**
	 * Makes this image a copy of another image.
	 *
	 * @param source Image from which to copy.
	 */
	image& operator=(const image& source);
	
	/**
	 * Returns an iterator to the first pixel.
	 *
	 * @tparam T Pixel data type.
	 */
	/// @{
	template <class T>
	inline constexpr T* begin() noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return static_cast<T*>(pixels);
	}
	template <class T>
	inline constexpr const T* begin() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return static_cast<const T*>(pixels);
	}
	template <class T>
	inline constexpr const T* cbegin() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return static_cast<const T*>(pixels);
	}
	/// @}
	
	/**
	 * Returns an iterator to the pixel following the last pixel.
	 *
	 * @tparam T Pixel data type.
	 */
	/// @{
	template <class T>
	inline constexpr T* end() noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<T*>(static_cast<unsigned char*>(pixels) + size);
	}
	template <class T>
	inline constexpr const T* end() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<const T*>(static_cast<const unsigned char*>(pixels) + size);
	}
	template <class T>
	inline constexpr const T* cend() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<const T*>(static_cast<const unsigned char*>(pixels) + size);
	}
	/// @}
	
	/**
	 * Checks whether another image has the same number of channels and pixel size as this image.
	 *
	 * @param other Image for with which to compare compatibility.
	 * @return `true` if the image formats are compatible, `false` otherwise.
	 */
	bool compatible(const image& other) const;
	
	/**
	 * Copies pixel data from another image with a compatible format into this image.
	 *
	 * @param source Source image from which to copy pixel data.
	 * @param w Width of the subimage to copy.
	 * @param h Height of the subimage to copy.
	 * @param from_x X-coordinate of the first pixel to copy from the source subimage.
	 * @param from_y Y-coordinate of the first pixel to copy from the source subimage.
	 * @param to_x X-coordinate of the first pixel in the destination subimage.
	 * @param to_y Y-coordinate of the first pixel in the destination subimage.
	 *
	 * @except std::runtime_error Cannot copy image with mismatched format.
	 *
	 * @see image::compatible(const image&) const
	 */
	void copy(const image& source, unsigned int w, unsigned int h, unsigned int from_x = 0, int unsigned from_y = 0, unsigned int to_x = 0, unsigned int to_y = 0);

	/**
	 * Changes the format of the image. Existing pixel data will be erased if the format has changed.
	 *
	 * @param component_size Size of channel components, in bytes.
	 * @param channel_count Number of channels in the image.
	 */
	void format(std::size_t component_size, std::size_t channel_count);

	/**
	 * Resizes the image. Existing pixel data will be erased if the size has changed.
	 *
	 * @param width New width of the image, in pixels.
	 * @param height New height of the image, in pixels.
	 */
	void resize(unsigned int width, unsigned int height);

	/// Returns the width of the image, in pixels.
	unsigned int get_width() const;

	/// Returns the height of the image, in pixels.
	unsigned int get_height() const;
	
	/// Returns the size of channel components, in bytes.
	std::size_t get_component_size() const;

	/// Returns the number of color channels in the image. 
	std::size_t get_channel_count() const;
	
	/// Returns a pointer to the pixel data.
	/// @{
	void* data() noexcept;
	const void* data() const noexcept;
	/// @}
	
	/// Returns the size of a single pixel, in bytes.
	std::size_t get_pixel_size() const;
	
	/// Returns the size of the image, in bytes.
	std::size_t get_size() const;

private:
	void allocate_pixels();
	void free_pixels();

	unsigned int width;
	unsigned int height;
	std::size_t component_size;
	std::size_t channel_count;
	void* pixels;
	std::size_t pixel_size;
	std::size_t size;
};

inline unsigned int image::get_width() const
{
	return width;
}

inline unsigned int image::get_height() const
{
	return height;
}

inline std::size_t image::get_component_size() const
{
	return component_size;
}

inline std::size_t image::get_channel_count() const
{
	return channel_count;
}

inline void* image::data() noexcept
{
	return pixels;
}

inline const void* image::data() const noexcept
{
	return pixels;
}

inline std::size_t image::get_pixel_size() const
{
	return pixel_size;
}

inline std::size_t image::get_size() const
{
	return size;
}

#endif // ANTKEEPER_IMAGE_HPP

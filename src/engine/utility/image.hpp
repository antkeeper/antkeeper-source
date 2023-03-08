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

#ifndef ANTKEEPER_UTILITY_IMAGE_HPP
#define ANTKEEPER_UTILITY_IMAGE_HPP

#include <engine/math/vector.hpp>
#include <cstdint>
#include <type_traits>
#include <vector>

/**
 * Stores basic image data.
 */
class image
{
public:
	/**
	 * Returns an iterator to the first pixel.
	 *
	 * @tparam T Pixel data type.
	 */
	/// @{
	template <class T>
	[[nodiscard]] inline constexpr T* begin() noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<T*>(pixels.data());
	}
	template <class T>
	[[nodiscard]] inline constexpr const T* begin() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<const T*>(pixels.data());
	}
	template <class T>
	[[nodiscard]] inline constexpr const T* cbegin() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<const T*>(pixels.data());
	}
	/// @}
	
	/**
	 * Returns an iterator to the pixel following the last pixel.
	 *
	 * @tparam T Pixel data type.
	 */
	/// @{
	template <class T>
	[[nodiscard]] inline constexpr T* end() noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<T*>(pixels.data() + pixels.size());
	}
	template <class T>
	[[nodiscard]] inline constexpr const T* end() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<const T*>(pixels.data() + pixels.size());
	}
	template <class T>
	[[nodiscard]] inline constexpr const T* cend() const noexcept
	{
		static_assert(std::is_standard_layout<T>::value, "Pixel iterator type is not standard-layout.");
		static_assert(std::is_trivial<T>::value, "Pixel iterator type is not trivial.");
		return reinterpret_cast<const T*>(pixels.data() + pixels.size());
	}
	/// @}
	
	/**
	 * Checks whether another image has the same number of channels and pixel size as this image.
	 *
	 * @param other Image for with which to compare compatibility.
	 * @return `true` if the image formats are compatible, `false` otherwise.
	 */
	[[nodiscard]] bool compatible(const image& other) const noexcept;
	
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
	void copy
	(
		const image& source,
		std::uint32_t w,
		std::uint32_t h,
		std::uint32_t from_x = 0,
		std::uint32_t from_y = 0,
		std::uint32_t to_x = 0,
		std::uint32_t to_y = 0
	);

	/**
	 * Changes the format of the image. Existing pixel data will be erased if the format has changed.
	 *
	 * @param component_size Size of channel components, in bytes.
	 * @param channel_count Number of channels in the image.
	 */
	void format(std::size_t component_size, std::uint8_t channel_count);

	/**
	 * Resizes the image. Existing pixel data will be erased if the size has changed.
	 *
	 * @param width New width of the image, in pixels.
	 * @param height New height of the image, in pixels.
	 */
	void resize(std::uint32_t width, std::uint32_t height);

	/// Returns the width of the image, in pixels.
	[[nodiscard]] inline std::uint32_t width() const noexcept
	{
		return m_width;
	}

	/// Returns the height of the image, in pixels.
	[[nodiscard]] inline std::uint32_t height() const noexcept
	{
		return m_height;
	}

	/// Returns the number of color channels in the image. 
	[[nodiscard]] inline std::uint8_t channel_count() const noexcept
	{
		return m_channel_count;
	}
	
	/// Returns a pointer to the pixel data.
	/// @{
	[[nodiscard]] inline const std::byte* data() const noexcept
	{
		return pixels.data();
	}
	[[nodiscard]] inline std::byte* data() noexcept
	{
		return pixels.data();
	}
	/// @}
	
	/// Returns the size of channel components, in bytes.
	[[nodiscard]] inline std::size_t component_size() const noexcept
	{
		return m_component_size;
	}
	
	/// Returns the size of a single pixel, in bytes.
	[[nodiscard]] inline std::size_t pixel_size() const noexcept
	{
		return m_pixel_size;
	}
	
	/// Returns the size of the image, in bytes.
	[[nodiscard]] inline std::size_t size() const noexcept
	{
		return pixels.size();
	}
	
private:
	std::uint32_t m_width{0};
	std::uint32_t m_height{0};
	std::uint8_t m_channel_count{0};
	std::size_t m_component_size{0};
	std::size_t m_pixel_size{0};
	std::vector<std::byte> pixels;
};

#endif // ANTKEEPER_UTILITY_IMAGE_HPP

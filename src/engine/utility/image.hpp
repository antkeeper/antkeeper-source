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
#include <vector>
#include <cstddef>

/**
 * Pixel data buffer.
 */
class image
{
public:
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
	 * @param dimensions Dimensions of the subimage to copy.
	 * @param from Coordinates of the first pixel to copy from the source subimage.
	 * @param to Coordinates of the first pixel in the destination subimage.
	 *
	 * @except std::runtime_error Cannot copy image with mismatched format.
	 *
	 * @see image::compatible(const image&) const
	 */
	void copy
	(
		const image& source,
		const math::uvec2& dimensions,
		const math::uvec2& from = {},
		const math::uvec2& to = {}
	);

	/**
	 * Changes the format of the image.
	 *
	 * @param channels Number of channels in the image.
	 * @param bit_depth Number of bits per channel.
	 *
	 * @warning Pre-existing pixel data will be invalidated.
	 * @warning Bit depth must be byte-aligned.
	 *
	 * @except std::runtime_error Image bit depth must be byte-aligned.
	 */
	void format(unsigned int channels, unsigned int bit_depth = 8u);
	
	/**
	 * Resizes the image.
	 *
	 * @param size New dimensions of the image, in pixels.
	 *
	 * @warning Pre-existing pixel data will be invalidated.
	 */
	/// @{
	inline void resize(unsigned int size)
	{
		resize(math::uvec3{size, 1u, 1u});
	}
	
	inline void resize(const math::uvec2& size)
	{
		resize(math::uvec3{size.x(), size.y(), 1u});
	}
	
	void resize(const math::uvec3& size);
	/// @}
	
	/// @name Pixel access
	/// @{
	
	/// Returns a pointer to the pixel data.
	/// @{
	[[nodiscard]] inline constexpr const std::byte* data() const noexcept
	{
		return m_data.data();
	}
	[[nodiscard]] inline constexpr std::byte* data() noexcept
	{
		return m_data.data();
	}
	/// @}
	
	/**
	 * Returns the value of a pixel.
	 *
	 * @tparam T Pixel data type.
	 *
	 * @param position Coordinates of a pixel.
	 *
	 * @return Pixel value.
	 */
	/// @{
	template <class T>
	[[nodiscard]] T get(unsigned int position) const
	{
		T value;
		std::memcpy(&value, data() + static_cast<std::size_t>(position) * m_pixel_stride, sizeof(T));
		return value;
	}
	
	template <class T>
	[[nodiscard]] T get(const math::uvec2& position) const
	{
		const auto index = static_cast<std::size_t>(position.y()) * m_size.x() + position.x();
		T value;
		std::memcpy(&value, data() + index * m_pixel_stride, sizeof(T));
		return value;
	}
	
	template <class T>
	[[nodiscard]] T get(const math::uvec3& position) const
	{
		const auto index = (static_cast<std::size_t>(position.z()) * m_size.y() + position.y()) * m_size.x() + position.x();
		T value;
		std::memcpy(&value, data() + index * m_pixel_stride, sizeof(T));
		return value;
	}
	/// @}
	
	/**
	 * Sets the value of a pixel.
	 *
	 * @tparam T Pixel data type.
	 *
	 * @param position Coordinates of a pixel.
	 * @param value Pixel value.
	 */
	/// @{
	template <class T>
	[[nodiscard]] void set(unsigned int position, const T& value)
	{
		std::memcpy(data() + static_cast<std::size_t>(position) * m_pixel_stride, &value, sizeof(T));
	}
	
	template <class T>
	[[nodiscard]] void set(const math::uvec2& position, const T& value)
	{
		const auto index = static_cast<std::size_t>(position.y()) * m_size.x() + position.x();
		std::memcpy(data() + index * m_pixel_stride, &value, sizeof(T));
	}
	
	template <class T>
	[[nodiscard]] void set(const math::uvec3& position, const T& value)
	{
		const auto index = (static_cast<std::size_t>(position.z()) * m_size.y() + position.y()) * m_size.x() + position.x();
		std::memcpy(data() + index * m_pixel_stride, &value, sizeof(T));
	}
	/// @}
	
	/**
	 * Samples a texel.
	 *
	 * @param position Coordinates of a pixel.
	 *
	 * @return RGBA texel, on `[0, 1]`.
	 */
	/// @{
	[[nodiscard]] inline math::fvec4 sample(unsigned int position) const
	{
		return sample(static_cast<std::size_t>(position));
	}
	
	[[nodiscard]] inline math::fvec4 sample(const math::uvec2& position) const
	{
		return sample(static_cast<std::size_t>(position.y()) * m_size.x() + position.x());
	}
	
	[[nodiscard]] inline math::fvec4 sample(const math::uvec3& position) const
	{
		return sample((static_cast<std::size_t>(position.z()) * m_size.y() + position.y()) * m_size.x() + position.x());
	}
	/// @}
	
	/// @}
	
	/// Returns the dimensions of the the image, in pixels.
	[[nodiscard]] inline constexpr const math::uvec3& size() const noexcept
	{
		return m_size;
	}
	
	/// Returns the number of channels in the image. 
	[[nodiscard]] inline constexpr unsigned int channels() const noexcept
	{
		return m_channels;
	}
	
	/// Returns the number of bits per channel in the image. 
	[[nodiscard]] inline constexpr unsigned int bit_depth() const noexcept
	{
		return m_bit_depth;
	}
	
	/// Returns the size of the image, in bytes.
	[[nodiscard]] inline constexpr std::size_t size_bytes() const noexcept
	{
		return m_data.size();
	}
	
private:
	[[nodiscard]] math::fvec4 sample(std::size_t index) const;
	
	math::uvec3 m_size{};
	unsigned int m_channels{};
	unsigned int m_bit_depth{};
	unsigned int m_pixel_stride{};
	float m_sample_scale{};
	std::vector<std::byte> m_data;
};

#endif // ANTKEEPER_UTILITY_IMAGE_HPP

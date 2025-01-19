// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_MORTON_HPP
#define ANTKEEPER_GEOM_MORTON_HPP

#include <concepts>

namespace geom {

/// Encodes 2D coordinates as a Morton location code.
/// @param[in] x X-coordinate to encode.
/// @param[in] y Y-coordinate to encode.
/// @return Morton location code.
template <std::unsigned_integral T>
constexpr T morton_encode(T x, T y) noexcept
{
	auto expand = [](T x) -> T
	{
		x &= (1 << (sizeof(T) << 2)) - 1;
		
		if constexpr(sizeof(T) >= 8)
		{
			x = (x ^ (x << 16)) & T(0x0000ffff0000ffff);
		}
		if constexpr(sizeof(T) >= 4)
		{
			x = (x ^ (x <<  8)) & T(0x00ff00ff00ff00ff);
		}
		if constexpr(sizeof(T) >= 2)
		{
			x = (x ^ (x <<  4)) & T(0x0f0f0f0f0f0f0f0f);
		}
		
		x = (x ^ (x << 2)) & T(0x3333333333333333);
		x = (x ^ (x << 1)) & T(0x5555555555555555);
		
		return x;
	};
	
	return expand(x) | (expand(y) << 1);
}

/// Encodes 3D coordinates as a Morton location code.
/// @param[in] x X-coordinate to encode.
/// @param[in] y Y-coordinate to encode.
/// @param[in] z Z-coordinate to encode.
/// @return Morton location code.
template <std::unsigned_integral T>
constexpr T morton_encode(T x, T y, T z) noexcept
{
	auto expand = [](T x) -> T
	{
		if constexpr(sizeof(T) == 1)
		{
			x &=               T(0x3);
			x = (x | x << 2) & T(0x9);
		}
		else if constexpr(sizeof(T) == 2)
		{
			x &=               T(0x001f);
			x = (x | x << 8) & T(0x100f);
			x = (x | x << 4) & T(0x10c3);
			x = (x | x << 2) & T(0x1249);
		}
		else if constexpr(sizeof(T) == 4)
		{
			x &=                T(0x00003ff);
			x = (x | x << 16) & T(0x30000ff);
			x = (x | x <<  8) & T(0x300f00f);
			x = (x | x <<  4) & T(0x30c30c3);
			x = (x | x <<  2) & T(0x9249249);
		}
		else if constexpr(sizeof(T) == 8)
		{
			x &=                T(0x00000000001fffff);
			x = (x | x << 32) & T(0x001f00000000ffff);
			x = (x | x << 16) & T(0x001f0000ff0000ff);
			x = (x | x <<  8) & T(0x100f00f00f00f00f);
			x = (x | x <<  4) & T(0x10c30c30c30c30c3);
			x = (x | x <<  2) & T(0x1249249249249249);
		}

		return x;
	};
	
	return expand(x) | (expand(y) << 1) | (expand(z) << 2);
}

/// Decodes 2D coordinates from a Morton location code.
/// @param[in] code Morton location code to decode.
/// @param[out] x Decoded x-coordinate.
/// @param[out] y Decoded y-coordinate.
template <std::unsigned_integral T>
void morton_decode(T code, T& x, T& y) noexcept
{
	auto compress = [](T x) -> T
	{
		x &=                 T(0x5555555555555555);
		x = (x ^ (x >> 1)) & T(0x3333333333333333);
		x = (x ^ (x >> 2)) & T(0x0f0f0f0f0f0f0f0f);
		
		if constexpr(sizeof(T) >= 2)
		{
			x = (x ^ (x >>  4)) & T(0x00ff00ff00ff00ff);
		}
		if constexpr(sizeof(T) >= 4)
		{
			x = (x ^ (x >>  8)) & T(0x0000ffff0000ffff);
		}
		if constexpr(sizeof(T) >= 8)
		{
			x = (x ^ (x >> 16)) & T(0x00000000ffffffff);
		}
		
		return x;
	};
	
	x = compress(code);
	y = compress(code >> 1);
}

/// Decodes 3D coordinates from a Morton location code.
/// @param[in] code Morton location code to decode.
/// @param[out] x Decoded x-coordinate.
/// @param[out] y Decoded y-coordinate.
/// @param[out] z Decoded z-coordinate.
template <std::unsigned_integral T>
void morton_decode(T code, T& x, T& y, T& z) noexcept
{
	auto compress = [](T x) -> T
	{
		if constexpr(sizeof(T) == 1)
		{
			x &=               T(0x9);
			x = (x ^ x >> 2) & T(0x3);
		}
		else if constexpr(sizeof(T) == 2)
		{
			x &=               T(0x1249);
			x = (x ^ x >> 2) & T(0x10c3);
			x = (x ^ x >> 4) & T(0x100f);
			x = (x ^ x >> 8) & T(0x001f);
		}
		else if constexpr(sizeof(T) == 4)
		{
			x &=                T(0x9249249);
			x = (x ^ x >>  2) & T(0x30c30c3);
			x = (x ^ x >>  4) & T(0x300f00f);
			x = (x ^ x >>  8) & T(0x30000ff);
			x = (x ^ x >> 16) & T(0x00003ff);
		}
		else if constexpr(sizeof(T) == 8)
		{
			x &=                T(0x1249249249249249);
			x = (x ^ x >>  2) & T(0x10c30c30c30c30c3);
			x = (x ^ x >>  4) & T(0x100f00f00f00f00f);
			x = (x ^ x >>  8) & T(0x001f0000ff0000ff);
			x = (x ^ x >> 16) & T(0x001f00000000ffff);
			x = (x ^ x >> 32) & T(0x00000000001fffff);
		}
		
		return x;
	};
	
	x = compress(code);
	y = compress(code >> 1);
	z = compress(code >> 2);
}

} // namespace geom

#endif // ANTKEEPER_GEOM_MORTON_HPP

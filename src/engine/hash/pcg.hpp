// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_HASH_PCG_HPP
#define ANTKEEPER_HASH_PCG_HPP

#include <engine/hash/make-uint.hpp>
#include <engine/math/vector.hpp>
#include <cstdint>
#include <type_traits>

// export module hash.pcg;
// import hash.make_uint;
// import math.vector;
// import <cstdint>;
// import <type_traits>;

namespace hash {

/// @private
template <class T>
constexpr T pcg_multiplier = 0;
template <>
constexpr std::uint8_t pcg_multiplier<std::uint8_t> = 141U;
template <>
constexpr std::uint16_t pcg_multiplier<std::uint16_t> = 12829U;
template <>
constexpr std::uint32_t pcg_multiplier<std::uint32_t> = 747796405UL;
template <>
constexpr std::uint64_t pcg_multiplier<std::uint64_t> = 6364136223846793005ULL;

/// @private
template <class T>
constexpr T pcg_increment = 0;
template <>
constexpr std::uint8_t pcg_increment<std::uint8_t> = 77U;
template <>
constexpr std::uint16_t pcg_increment<std::uint16_t> = 47989U;
template <>
constexpr std::uint32_t pcg_increment<std::uint32_t> = 2891336453UL;
template <>
constexpr std::uint64_t pcg_increment<std::uint64_t> = 1442695040888963407ULL;

/// @private
template <class T>
constexpr T mcg_multiplier = 0;
template <>
constexpr std::uint8_t mcg_multiplier<std::uint8_t> = 217U;
template <>
constexpr std::uint16_t mcg_multiplier<std::uint16_t> = 62169U;
template <>
constexpr std::uint32_t mcg_multiplier<std::uint32_t> = 277803737UL;
template <>
constexpr std::uint64_t mcg_multiplier<std::uint64_t> = 12605985483714917081ULL;

/// @private
template <class T>
[[nodiscard]] constexpr T pcg_uint(T x) noexcept
{
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
	static_assert(sizeof(T) <= 8);
	
	x = x * pcg_multiplier<T> + pcg_increment<T>;
	x = (x ^ (x >> ((x >> ((sizeof(T) * 8) - (sizeof(T) + 1))) + (sizeof(T) + 1)))) * mcg_multiplier<T>;
	return x ^ (x >> ((sizeof(T) * 16 + 2) / 3));
}

/// @private
template <class T>
[[nodiscard]] inline constexpr math::vector<T, 1> pcg_uvec1(math::vector<T, 1> x) noexcept
{
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
	static_assert(sizeof(T) <= 8);

	x[0] = T(x[0]);
	
	return x;
}

/// @private
template <class T>
[[nodiscard]] constexpr math::vector<T, 2> pcg_uvec2(math::vector<T, 2> x) noexcept
{
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
	static_assert(sizeof(T) <= 8);
	
	x = x * pcg_multiplier<T> + pcg_increment<T>;
	
    x[0] += x[1] * pcg_multiplier<T>;
    x[1] += x[0] * pcg_multiplier<T>;

	x[0] ^= x[0] >> (sizeof(T) * 4);
	x[1] ^= x[1] >> (sizeof(T) * 4);
	
    x[0] += x[1] * pcg_multiplier<T>;
    x[1] += x[0] * pcg_multiplier<T>;
	
	x[0] ^= x[0] >> (sizeof(T) * 4);
	x[1] ^= x[1] >> (sizeof(T) * 4);
	
	return x;
}

/// @private
template <class T>
[[nodiscard]] constexpr math::vector<T, 3> pcg_uvec3(math::vector<T, 3> x) noexcept
{
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
	static_assert(sizeof(T) <= 8);
	
	x = x * pcg_multiplier<T> + pcg_increment<T>;
	
	x[0] += x[1] * x[2];
	x[1] += x[2] * x[0];
	x[2] += x[0] * x[1];
	
	x[0] ^= x[0] >> (sizeof(T) * 4);
	x[1] ^= x[1] >> (sizeof(T) * 4);
	x[2] ^= x[2] >> (sizeof(T) * 4);
	
	x[0] += x[1] * x[2];
	x[1] += x[2] * x[0];
	x[2] += x[0] * x[1];
	
	return x;
}

/// @private
template <class T>
[[nodiscard]] constexpr math::vector<T, 4> pcg_uvec4(math::vector<T, 4> x) noexcept
{
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value);
	static_assert(sizeof(T) <= 8);
	
	x = x * pcg_multiplier<T> + pcg_increment<T>;
	
	x[0] += x[1] * x[3];
	x[1] += x[2] * x[0];
	x[2] += x[0] * x[1];
	x[3] += x[1] * x[2];
	
	x[0] ^= x[0] >> (sizeof(T) * 4);
	x[1] ^= x[1] >> (sizeof(T) * 4);
	x[2] ^= x[2] >> (sizeof(T) * 4);
	x[3] ^= x[3] >> (sizeof(T) * 4);
	
	x[0] += x[1] * x[3];
	x[1] += x[2] * x[0];
	x[2] += x[0] * x[1];
	x[3] += x[1] * x[2];
	
	return x;
}

/// PCG hash function.
/// @param x Input value.
/// @return Unsigned pseudorandom output value.
/// @warning Floating point and signed input values will be converted to unsigned integers via `static_cast`.
/// @warning Vectors with more than 4 elements are not supported.
/// 
/// @see https://en.wikipedia.org/wiki/Permuted_congruential_generator
/// @see O'Neill, M.E. (2014). PCG : A Family of Simple Fast Space-Efficient Statistically Good Algorithms for Random Number Generation.
/// @see Mark Jarzynski and Marc Olano, Hash Functions for GPU Rendering, Journal of Computer Graphics Techniques (JCGT), vol. 9, no. 3, 21-38, 2020.
[[nodiscard]] inline constexpr std::uint8_t pcg(std::uint8_t x) noexcept
{
	return pcg_uint<std::uint8_t>(x);
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint16_t pcg(std::uint16_t x) noexcept
{
	return pcg_uint<std::uint16_t>(x);
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint32_t pcg(std::uint32_t x) noexcept
{
	return pcg_uint<std::uint32_t>(x);
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint64_t pcg(std::uint64_t x) noexcept
{
	return pcg_uint<std::uint64_t>(x);
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint8_t pcg(std::int8_t x) noexcept
{
	return pcg_uint<std::uint8_t>(static_cast<std::uint8_t>(x));
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint16_t pcg(std::int16_t x) noexcept
{
	return pcg_uint<std::uint16_t>(static_cast<std::uint16_t>(x));
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint32_t pcg(std::int32_t x) noexcept
{
	return pcg_uint<std::uint32_t>(static_cast<std::uint32_t>(x));
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint64_t pcg(std::int64_t x) noexcept
{
	return pcg_uint<std::uint64_t>(static_cast<std::uint64_t>(x));
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint32_t pcg(float x) noexcept
{
	return pcg_uint<std::uint32_t>(static_cast<std::uint32_t>(x));
}

/// @copydoc pcg(std::uint8_t)
[[nodiscard]] inline constexpr std::uint64_t pcg(double x) noexcept
{
	return pcg_uint<std::uint64_t>(static_cast<std::uint64_t>(x));
}

/// @copydoc pcg(std::uint8_t)
template <class T, std::size_t N>
[[nodiscard]] inline constexpr math::vector<make_uint_t<T>, N> pcg(const math::vector<T, N>& x) noexcept
{
	static_assert(N > 0 && N < 5, "PCG hash only supports vectors with 1-4 elements.");
	
	if constexpr (N == 1)
	{
		return pcg_uvec1<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
	}
	else if constexpr (N == 2)
	{
		return pcg_uvec2<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
	}
	else if constexpr (N == 3)
	{
		return pcg_uvec3<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
	}
	else
	{
		return pcg_uvec4<make_uint_t<T>>(math::vector<make_uint_t<T>, N>(x));
	}
}

} // namespace hash

#endif // ANTKEEPER_HASH_PCG_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_VECTOR_CONSTANTS_HPP
#define ANTKEEPER_MATH_VECTOR_CONSTANTS_HPP

#include <engine/math/vector-types.hpp>
#include <engine/math/constants.hpp>
#include <cstddef>

// export module math.vector:constants;
// import :type;
// import math.constants;
// import <cstddef>;

namespace math {
namespace constants {

/// @private
template <class T, std::size_t N>
[[nodiscard]] inline consteval vector<T, N> fill_vector(const T& value) noexcept
{
	vector<T, N> v;
	v.fill(value);
	return v;
}

/// @name Vector constants
/// @{

/// Vector of zeros.
template <class T, std::size_t N>
inline constexpr vector<T, N> zero<vector<T, N>> = {};

/// Vector of ones.
template <class T, std::size_t N>
inline constexpr vector<T, N> one<vector<T, N>> = fill_vector<T, N>(one<T>);

/// Vector of infinities.
template <class T, std::size_t N>
inline constexpr vector<T, N> inf<vector<T, N>> = fill_vector<T, N>(inf<T>);

/// @}

} // namespace constants

// Bring math::constants into math namespace
using namespace constants;

} // namespace math

#endif // ANTKEEPER_MATH_VECTOR_CONSTANTS_HPP

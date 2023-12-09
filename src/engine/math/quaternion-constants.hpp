// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_QUATERNION_CONSTANTS_HPP
#define ANTKEEPER_MATH_QUATERNION_CONSTANTS_HPP

#include <engine/math/quaternion-types.hpp>
#include <engine/math/constants.hpp>

// export module math.quaternion:constants;
// import :type;
// import math.constants;

namespace math {
namespace constants {

/// @name Quaternion constants
/// @{

/// Quaternion of zeros.
template <class T>
inline constexpr quaternion<T> zero<quaternion<T>> = {};

/// Quaternion of ones.
template <class T>
inline constexpr quaternion<T> one<quaternion<T>> = {one<T>, {one<T>, one<T>, one<T>}};

/// Quaternion of infinities.
template <class T>
inline constexpr quaternion<T> inf<quaternion<T>> = {inf<T>, {inf<T>, inf<T>, inf<T>}};

/// Rotational identity quaternion.
template <class T>
inline constexpr quaternion<T> identity<quaternion<T>> = {one<T>, {}};

/// @}

} // namespace constants

// Bring math::constants into math namespace
using namespace constants;

} // namespace math

#endif // ANTKEEPER_MATH_QUATERNION_CONSTANTS_HPP

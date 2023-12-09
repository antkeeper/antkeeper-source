// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_QUATERNION_CONSTANTS_HPP
#define ANTKEEPER_MATH_QUATERNION_CONSTANTS_HPP

#include <engine/math/quaternion-type.hpp>
#include <engine/math/numbers.hpp>

// export module math.quaternion:constants;
// import :type;
// import math.numbers;

namespace math
{
	namespace numbers
	{
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
	}
}

#endif // ANTKEEPER_MATH_QUATERNION_CONSTANTS_HPP

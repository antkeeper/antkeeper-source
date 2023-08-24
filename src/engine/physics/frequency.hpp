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

#ifndef ANTKEEPER_PHYSICS_FREQUENCY_HPP
#define ANTKEEPER_PHYSICS_FREQUENCY_HPP

#include <engine/math/numbers.hpp>

namespace physics {

/**
 * Converts frequency to angular frequency.
 *
 * @tparam T Scalar type.
 *
 * @param f Frequency, in hertz.
 *
 * @return Angular frequency, in radians per second.
 */
template <class T>
[[nodiscard]] inline constexpr T hz_to_rads(T f) noexcept
{
	return f * math::two_pi<T>;
}

/**
 * Converts angular frequency to frequency.
 *
 * @tparam T Scalar type.
 *
 * @param w Angular frequency, in radians per second.
 *
 * @return Frequency, in hertz.
 */
template <class T>
[[nodiscard]] inline constexpr T rads_to_hz(T w) noexcept
{
	return w / math::two_pi<T>;
}

/**
 * Converts oscillation period to angular frequency.
 *
 * @tparam T Scalar type.
 *
 * @param t Oscillation period, in seconds.
 *
 * @return Angular frequency, in radians per second.
 */
template <class T>
[[nodiscard]] inline constexpr T s_to_rads(T t) noexcept
{
	return math::two_pi<T> / t;
}

/**
 * Converts angular frequency to oscillation period.
 *
 * @tparam T Scalar type.
 *
 * @param w Angular frequency, in radians per second.
 *
 * @return Oscillation period, in seconds.
 */
template <class T>
[[nodiscard]] inline constexpr T rads_to_s(T w) noexcept
{
	return math::two_pi<T> / w;
}

} // namespace physics

#endif // ANTKEEPER_PHYSICS_FREQUENCY_HPP

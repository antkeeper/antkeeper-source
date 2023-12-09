// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_FREQUENCY_HPP
#define ANTKEEPER_PHYSICS_FREQUENCY_HPP

#include <engine/math/constants.hpp>

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

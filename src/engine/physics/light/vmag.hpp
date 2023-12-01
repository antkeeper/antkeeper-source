// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_LIGHT_VMAG_HPP
#define ANTKEEPER_PHYSICS_LIGHT_VMAG_HPP

#include <cmath>

namespace physics {
namespace light {

/// Apparent (visual) magnitude functions.
namespace vmag {

/**
 * Converts apparent magnitude to a brightness factor relative to a 0th magnitude star.
 *
 * @param mv Apparent magnitude.
 * @return Brightness factor relative to a 0th magnitude star.
 *
 * @see https://en.wikipedia.org/wiki/Illuminance
 */
template <class T>
T to_brightness(T mv)
{
	// 100^(1/5)
	static constexpr T fifth_root_100 = T{2.5118864315095801110850320677993};
	return std::pow(fifth_root_100, -mv);
}

/**
 * Converts apparent magnitude to illuminance.
 *
 * @param mv Apparent magnitude.
 * @return Illuminance, in lux.
 *
 * @see https://en.wikipedia.org/wiki/Illuminance
 */
template <class T>
T to_illuminance(T mv)
{
	return std::pow(T{10}, (T{-14.18} - mv) * T{0.4});
}

/**
 * Converts illuminance to apparent  magnitude.
 *
 * @param ev Illuminance, in lux.
 * @return Apparent magnitude.
 *
 * @see https://en.wikipedia.org/wiki/Illuminance
 */
template <class T>
T from_illuminance(T ev)
{
	return T{-14.18} - T{2.5} * std::log10(ev);
}

} // namespace vmag
} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_VMAG_HPP

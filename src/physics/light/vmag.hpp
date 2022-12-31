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
	static constexpr double fifth_root_100 = 2.5118864315095801110850320677993;
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
	return std::pow(10.0, (-14.18 - mv) * 0.4);
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
	return -14.18 - 2.5 * std::log10(ev);
}

} // namespace vmag
} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_VMAG_HPP

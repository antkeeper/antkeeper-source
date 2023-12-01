// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ASTRO_APPARENT_SIZE_HPP
#define ANTKEEPER_ASTRO_APPARENT_SIZE_HPP

#include <cmath>

namespace astro
{

/**
 * Finds the angular radius of a celestial object, given its radius and distance.
 *
 * @param radius Radius of the celestial object.
 * @param distance Distance to the celestial object.
 * @return Angular radius, in radians.
 */
template <class T>
T angular_radius(T radius, T distance)
{
	return std::asin(radius / distance);
}

} // namespace astro

#endif // ANTKEEPER_ASTRO_APPARENT_SIZE_HPP

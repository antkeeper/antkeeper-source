/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_PHYSICS_LIGHT_BLACKBODY_HPP
#define ANTKEEPER_PHYSICS_LIGHT_BLACKBODY_HPP

#include "math/constants.hpp"
#include "physics/constants.hpp"
#include "physics/planck.hpp"

namespace physics {
namespace light {

/**
 * Blackbody radiation functions.
 */
namespace blackbody {

/**
 * Calculates the radiant exitance of a blackbody.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @return Radiant exitance of the blackbody, in watt per square meter.
 *
 * @see https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_law
 * @see https://en.wikipedia.org/wiki/Radiant_exitance
 */
template <class T>
T radiant_exitance(T t);

/**
 * Calculates the radiant flux of a blackbody.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param a Surface area of the blackbody, in square meters.
 * @return Radiant power of the blackbody, in watts.
 *
 * @see https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_law
 */
template <class T>
T radiant_flux(T t, T a);

/**
 * Calculates the spectral radiance of a blackbody at a given wavelength.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param lambda Wavelength of light, in meters.
 * @param c Speed of light in medium.
 * @return Spectral radiance, in watt per steradian per square meter per meter.
 *
 * @see physics::planck::wavelength
 */
template <class T>
constexpr auto spectral_radiance = planck::wavelength<T>;

template <class T>
T radiant_exitance(T t)
{
	const T tt = t * t;
	return constants::stefan_boltzmann<T> * (tt * tt);
}

template <class T>
T radiant_flux(T t, T a)
{
	return a * radiant_exitance(t);
}

} // namespace blackbody

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_BLACKBODY_HPP

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

#ifndef ANTKEEPER_PHYSICS_PLANCK_HPP
#define ANTKEEPER_PHYSICS_PLANCK_HPP

#include "physics/constants.hpp"
#include <cmath>

namespace physics {

/// Various forms of Planck's law.
namespace planck {

/**
 * Wavelength variant of Planck's law.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param lambda Wavelength of light, in meters.
 * @param c Speed of light in medium.
 * @return Spectral radiance, in watt per steradian per square meter per meter.
 */
template <class T>
T wavelength(T t, T lambda, T c = constants::speed_of_light<T>);

template <class T>
T wavelength(T t, T lambda, T c)
{
	const T hc = constants::planck<T> * c;
	const T lambda2 = lambda * lambda;
	
	// First radiation constant (c1L)
	const T c1 = T(2) * hc * c;
	
	// Second radiation constant (c2)
	const T c2 = hc / constants::boltzmann<T>;
	
	return (c1 / (lambda2 * lambda2 * lambda)) / std::expm1(c2 / (lambda * t));
}

} // namespace planck

} // namespace physics

#endif // ANTKEEPER_PHYSICS_PLANCK_HPP

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

#ifndef ANTKEEPER_PHYSICS_LIGHT_BLACKBODY_HPP
#define ANTKEEPER_PHYSICS_LIGHT_BLACKBODY_HPP

#include <engine/math/numbers.hpp>
#include <engine/physics/constants.hpp>

namespace physics {
namespace light {

/**
 * Blackbody radiation functions.
 *
 * @see https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_law
 */
namespace blackbody {

/**
 * Calculates the radiant exitance of a blackbody.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @return Radiant exitance of the blackbody, in watt per square meter.
 */
template <class T>
T radiant_exitance(T t);

/**
 * Calculates the radiant flux of a blackbody.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param a Surface area of the blackbody, in square meters.
 * @return Radiant flux of the blackbody, in watt.
 */
template <class T>
T radiant_flux(T t, T a);

/**
 * Calculates the radiant intensity of a blackbody.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param a Surface area of the blackbody, in square meters.
 * @param omega Solid angle, in steradians.
 * @return Radiant intensity of the blackbody, in watt per steradian.
 */
template <class T>
T radiant_intensity(T t, T a, T omega);

/**
 * Calculates the spectral exitance of a blackbody for the given wavelength.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param lambda Wavelength of light, in meters.
 * @param c Speed of light in medium.
 * @return Spectral exitance, in watt per square meter, per meter.
 */
template <class T>
T spectral_exitance(T t, T lambda, T c = constants::speed_of_light<T>);

/**
 * Calculates the spectral flux of a blackbody for the given wavelength.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param a Surface area of the blackbody, in square meters.
 * @param lambda Wavelength of light, in meters.
 * @param c Speed of light in medium.
 * @return Spectral flux of the blackbody, in watt per meter.
 */
template <class T>
T spectral_flux(T t, T a, T lambda, T c  = constants::speed_of_light<T>);

/**
 * Calculates the spectral intensity of a blackbody for the given wavelength.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param a Surface area of the blackbody, in square meters.
 * @param lambda Wavelength of light, in meters.
 * @param omega Solid angle, in steradians.
 * @param c Speed of light in medium.
 * @return Spectral intensity of the blackbody for the given wavelength, in watt per steradian per meter.
 */
template <class T>
T spectral_intensity(T t, T a, T lambda, T omega, T c  = constants::speed_of_light<T>);

/**
 * Calculates the spectral radiance of a blackbody for the given wavelength.
 *
 * @param t Temperature of the blackbody, in kelvin.
 * @param lambda Wavelength of light, in meters.
 * @param c Speed of light in medium.
 * @return Spectral radiance, in watt per steradian per square meter per meter.
 */
template <class T>
T spectral_radiance(T t, T lambda, T c = constants::speed_of_light<T>);

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

template <class T>
T radiant_intensity(T t, T a, T omega)
{
	return radiant_flux(t, a) / omega;
}

template <class T>
T spectral_exitance(T t, T lambda, T c)
{
	const T hc = constants::planck<T> * c;
	const T lambda2 = lambda * lambda;
	
	// First radiation constant (c1)
	const T c1 = T(2) * math::pi<T> * hc * c;
	
	// Second radiation constant (c2)
	const T c2 = hc / constants::boltzmann<T>;
	
	return (c1 / (lambda2 * lambda2 * lambda)) / std::expm1(c2 / (lambda * t));
}

template <class T>
T spectral_flux(T t, T a, T lambda, T c)
{
	return a * spectral_exitance(t, lambda, c);
}

template <class T>
T spectral_intensity(T t, T a, T lambda, T omega, T c)
{
	return spectral_flux(t, a, lambda, c) / omega;
}

template <class T>
T spectral_radiance(T t, T lambda, T c)
{
	const T hc = constants::planck<T> * c;
	const T lambda2 = lambda * lambda;
	
	// First radiation constant (c1L)
	const T c1l = T(2) * hc * c;
	
	// Second radiation constant (c2)
	const T c2 = hc / constants::boltzmann<T>;
	
	return (c1l / (lambda2 * lambda2 * lambda)) / std::expm1(c2 / (lambda * t));
}

} // namespace blackbody

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_BLACKBODY_HPP

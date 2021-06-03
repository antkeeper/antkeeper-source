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

#ifndef ANTKEEPER_PHYSICS_ATMOSPHERE_HPP
#define ANTKEEPER_PHYSICS_ATMOSPHERE_HPP

#include "physics/constants.hpp"
#include "math/constants.hpp"
#include <cmath>

namespace physics {

/// Atmosphere-related functions.
namespace atmosphere {

/**
 * Calculates the density of exponentially-distributed atmospheric particles at a given altitude.
 *
 * @param d0 Density at sea level.
 * @param z Height above sea level.
 * @param sh Scale height of the particle type.
 * @return Particle density at altitude.
 *
 * @see https://en.wikipedia.org/wiki/Scale_height
 * @see https://en.wikipedia.org/wiki/Barometric_formula
 */
template <class T>
T density(T d0, T z, T sh)
{
	return d0 * std::exp(-z / sh);
}

/**
 * Calculates a particle polarizability factor used in computing scattering coefficients.
 *
 * @param ior Atmospheric index of refraction.
 * @param density Molecular density.
 * @return Polarization factor.
 *
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 * @see Real-Time Spectral Scattering in Large-Scale Natural Participating Media.
 */
template <class T>
T polarization(T ior, T density)
{
	const T ior2m1 = ior * ior - T(1.0);
	const T num = T(2) * math::pi<T> * math::pi<T> * ior2m1 * ior2m1;
	const T den = T(3) * density * density;
	return num / den;
}

/**
 * Calculates a Rayleigh scattering coefficient at sea level (wavelength-dependent).
 *
 * @param wavelength Wavelength of light, in meters.
 * @param density Molecular density of Rayleigh particles at sea level.
 * @param polarization Rayleigh particle polarization factor.
 *
 * @see atmosphere::polarization
 *
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 * @see Real-Time Spectral Scattering in Large-Scale Natural Participating Media.
 */
template <class T>
T scattering_rayleigh(T wavelength, T density, T polarization)
{
	const T wavelength2 = wavelength * wavelength;
	return T(4) * math::pi<T> * density / (wavelength2 * wavelength2) * polarization;
}

/**
 * Calculates a Mie scattering coefficient at sea level (wavelength-independent).
 *
 * @param density Molecular density of Mie particles at sea level.
 * @param polarization Mie particle polarization factor.
 *
 * @see atmosphere::polarization
 *
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 * @see Real-Time Spectral Scattering in Large-Scale Natural Participating Media.
 */
template <class T>
T scattering_mie(T density, T polarization)
{
	return T(4) * math::pi<T> * density * polarization;
}

/**
 * Calculates attenuation due to extinction (absorption + out-scattering).
 *
 * @param ec Extinction coefficient (absorption coefficient + scattering coefficient).
 * @param s Scale factor.
 * @return Attenuation factor.
 */
template <class T>
T extinction(T ec, T s)
{
	return std::exp(-(ec * s));
}

/**
 * Calculates the single-scattering albedo (SSA) given a scattering coefficient and an extinction coefficient.
 *
 * @param s Scattering coefficient.
 * @param e Extinction coefficient.
 * @return Single-scattering albedo.
 */
template <class T>
T albedo(T s, T e)
{
	return s / t;
}

/**
 * Approximates the optical depth of exponentially-distributed atmospheric particles between two points using the trapezoidal rule.
 *
 * @param a Start point.
 * @param b End point.
 * @param r Radius of the planet.
 * @param sh Scale height of the atmospheric particles.
 * @param n Number of samples.
 * @return Optical depth between @p a and @p b.
 */
template <class T>
T optical_depth(const math::vector3<T>& a, const math::vector3<T>& b, T r, T sh, std::size_t n)
{
	sh = T(-1) / sh;
	
	const T h = math::length(b - a) / T(n);
	
	math::vector3<T> dy = (b - a) / T(n);
	math::vector3<T> y = a + dy;
	
	T f_x = std::exp((math::length(a) - r) * sh);
	T f_y = std::exp((math::length(y) - r) * sh);
	T sum = (f_x + f_y);
	
	for (std::size_t i = 1; i < n; ++i)
	{
		f_x = f_y;
		y += dy;
		f_y = std::exp((math::length(y) - r) * sh);
		sum += (f_x + f_y);
	}
	
	return sum / T(2) * h;
}

} // namespace atmosphere

} // namespace physics

#endif // ANTKEEPER_PHYSICS_ATMOSPHERE_HPP

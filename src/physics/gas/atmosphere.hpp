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

#ifndef ANTKEEPER_PHYSICS_GAS_ATMOSPHERE_HPP
#define ANTKEEPER_PHYSICS_GAS_ATMOSPHERE_HPP

#include "physics/constants.hpp"
#include "math/constants.hpp"
#include <algorithm>
#include <cmath>

namespace physics {
namespace gas {

/// Atmosphere-related functions.
namespace atmosphere {

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
 * Calculates a Rayleigh scattering coefficient (wavelength-dependent).
 *
 * @param wavelength Wavelength of light, in meters.
 * @param density Molecular density of Rayleigh particles.
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
	return math::four_pi<T> * density / (wavelength2 * wavelength2) * polarization;
}

/**
 * Calculates a Mie scattering coefficient (wavelength-independent).
 *
 * @param density Molecular density of Mie particles.
 * @param polarization Mie particle polarization factor.
 *
 * @return Mie scattering coefficient.
 *
 * @see atmosphere::polarization
 *
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 * @see Real-Time Spectral Scattering in Large-Scale Natural Participating Media.
 */
template <class T>
T scattering_mie(T density, T polarization)
{
	return math::four_pi<T> * density * polarization;
}

/**
 * Calculates a Mie absorption coefficient (wavelength-independent).
 *
 * @param scattering Mie scattering coefficient.
 *
 * @return Mie absorption coefficient.
 *
 * @see Bruneton, E. and Neyret, F. (2008), Precomputed Atmospheric Scattering. Computer Graphics Forum, 27: 1079-1086. https://doi.org/10.1111/j.1467-8659.2008.01245.x
 */
template <class T>
T absorption_mie(T scattering)
{
	return scattering / T(9);
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
	return s / e;
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
T optical_depth_exp(const math::vector3<T>& a, const math::vector3<T>& b, T r, T sh, std::size_t n)
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

/**
 * Approximates the optical depth of triangularly-distributed atmospheric particles between two points using the trapezoidal rule.
 *
 * @param p0 Start point.
 * @param p1 End point.
 * @param r Radius of the planet.
 * @param a Distribution lower limit.
 * @param b Distribution upper limit.
 * @param c Distribution upper mode.
 * @param n Number of samples.
 * @return Optical depth between @p a and @p b.
 */
template <class T>
T optical_depth_tri(const math::vector3<T>& p0, const math::vector3<T>& p1, T r, T a, T b, T c, std::size_t n)
{
	a = T(1) / (a - c);
	b = T(1) / (b - c);
	
	const T h = math::length(p1 - p0) / T(n);
	
	math::vector3<T> dy = (p1 - p0) / T(n);
	math::vector3<T> y = p0 + dy;
	
	T z = math::length(p0) - r;
	T f_x = std::max(T(0), std::max(T(0), c - z) * a - std::max(T(0), z - c) * b + T(1));
	
	z = math::length(y) - r;
	T f_y = std::max(T(0), std::max(T(0), c - z) * a - std::max(T(0), z - c) * b + T(1));
	T sum = (f_x + f_y);
	
	for (std::size_t i = 1; i < n; ++i)
	{
		f_x = f_y;
		y += dy;
		
		z = math::length(y) - r;
		f_y = std::max(T(0), std::max(T(0), c - z) * a - std::max(T(0), z - c) * b + T(1));
		
		sum += (f_x + f_y);
	}
	
	return sum / T(2) * h;
}

/// Atmospheric density functions.
namespace density {

	/**
	 * Calculates the density of exponentially-distributed atmospheric particles at a given elevation.
	 *
	 * @param d0 Density at sea level.
	 * @param z Height above sea level.
	 * @param sh Scale height of the particle type.
	 *
	 * @return Particle density at elevation @p z.
	 *
	 * @see https://en.wikipedia.org/wiki/Barometric_formula
	 * @see https://en.wikipedia.org/wiki/Scale_height
	 */
	template <class T>
	T exponential(T d0, T z, T sh)
	{
		return d0 * std::exp(-z / sh);
	}
	
	/**
	 * Calculates the density of triangularly-distributed atmospheric particles at a given elevation.
	 *
	 * @param d0 Density at sea level.
	 * @param z Height above sea level.
	 * @param a Distribution lower limit.
	 * @param b Distribution upper limit.
	 * @param c Distribution mode.
	 *
	 * @return Particle density at elevation @p z.
	 *
	 * @see https://en.wikipedia.org/wiki/Triangular_distribution
	 */
	template <class T>
	T triangular(T d0, T z, T a, T b, T c)
	{
		return d0 * max(T(0), max(T(0), c - z) / (a - c) - max(T(0), z - c) / (b - c) + T(1));
	}

} // namespace density

} // namespace atmosphere

} // namespace gas
} // namespace physics

#endif // ANTKEEPER_PHYSICS_GAS_ATMOSPHERE_HPP

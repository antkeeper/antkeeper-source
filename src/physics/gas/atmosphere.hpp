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
 * Calculates a particle polarizability factor.
 *
 * @param ior Atmospheric index of refraction.
 * @param density Molecular number density, in mol/m-3.
 * @return Polarizability factor.
 *
 * @see Elek, O., & Kmoch, P. (2010). Real-time spectral scattering in large-scale natural participating media. Proceedings of the 26th Spring Conference on Computer Graphics - SCCG ’10. doi:10.1145/1925059.1925074
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 */
template <class T>
T polarization(T ior, T density)
{
	constexpr T k = T(2) * math::pi<T> * math::pi<T>;
	const T ior2m1 = ior * ior - T(1);
	const T num = k * ior2m1 * ior2m1;
	const T den = T(3) * density * density;
	return num / den;
}

/**
 * Calculates a wavelength-dependent scattering coefficient.
 *
 * @param density Molecular number density of the particles, in mol/m-3.
 * @param polarization Particle polarizability factor.
 * @param wavelength Wavelength of light, in meters.
 *
 * @return Scattering coefficient.
 *
 * @see atmosphere::polarization
 *
 * @see Elek, O., & Kmoch, P. (2010). Real-time spectral scattering in large-scale natural participating media. Proceedings of the 26th Spring Conference on Computer Graphics - SCCG ’10. doi:10.1145/1925059.1925074
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 */
template <class T>
T scattering(T density, T polarization, T wavelength)
{
	const T wavelength2 = wavelength * wavelength;
	return math::four_pi<T> * (density / (wavelength2 * wavelength2)) * polarization;
}

/**
 * Calculates a wavelength-independent scattering coefficient.
 *
 * @param density Molecular number density of the particles, in mol/m-3.
 * @param polarization Particle polarizability factor.
 *
 * @return Scattering coefficient.
 *
 * @see atmosphere::polarization
 *
 * @see Elek, O., & Kmoch, P. (2010). Real-time spectral scattering in large-scale natural participating media. Proceedings of the 26th Spring Conference on Computer Graphics - SCCG ’10. doi:10.1145/1925059.1925074
 * @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
 */
template <class T>
T scattering(T density, T polarization)
{
	return math::four_pi<T> * density * polarization;
}

/**
 * Calculates an absorption coefficient.
 *
 * @param scattering Scattering coefficient.
 * @param albedo Single-scattering albedo.
 *
 * @return Absorption coefficient.
 *
 * @see https://en.wikipedia.org/wiki/Single-scattering_albedo
 */
template <class T>
T absorption(T scattering, T albedo)
{
	return scattering * (T(1) / albedo - T(1));
}

/**
 * Calculates an extinction coefficient.
 *
 * @param scattering Scattering coefficient.
 * @param albedo Single-scattering albedo.
 *
 * @return Extinction coefficient.
 *
 * @see https://en.wikipedia.org/wiki/Single-scattering_albedo
 */
template <class T>
T extinction(T scattering, T albedo)
{
	return scattering / albedo;
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

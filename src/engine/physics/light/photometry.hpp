// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_LIGHT_PHOTOMETRY_HPP
#define ANTKEEPER_PHYSICS_LIGHT_PHOTOMETRY_HPP

#include <engine/math/numbers.hpp>
#include <engine/math/quadrature.hpp>

namespace physics {
namespace light {

/// Maximum luminous efficacy of an ideal monochromatic source, in lumen per watt.
template <class T>
constexpr T max_luminous_efficacy = T(683.002);

/**
 * Calculates the luminous efficiency of a light source.
 *
 * @param spd Unary function object that returns spectral radiance given a wavelength.
 * @param lef Unary function object that returns luminous efficiency given a wavelength.
 * @param first,last Range of sample wavelengths.
 * @return Luminous efficiency, on `[0, 1]`.
 *
 * @see physics::light::blackbody::spectral_radiance
 * @see physics::light::luminosity::photopic
 */
template <class T, class UnaryOp1, class UnaryOp2, class InputIt>
T luminous_efficiency(UnaryOp1 spd, UnaryOp2 lef, InputIt first, InputIt last)
{
	auto spd_lef = [spd, lef](T x) -> T
	{
		return spd(x) * lef(x);
	};
	
	const T num = math::quadrature::simpson(spd_lef, first, last);
	const T den = math::quadrature::simpson(spd, first, last);
	
	return num / den;
}

/**
 * Calculates luminous efficacy given luminous efficiency.
 *
 * @param efficiency Luminous efficiency, on `[0, 1]`.
 * @return Luminous flux, in lumens.
 */
template <class T>
T luminous_efficacy(T efficiency)
{
	return max_luminous_efficacy<T> * efficiency;
}

/**
 * Converts watts (radiant flux) to lumens (luminous flux).
 *
 * @param radiant_flux Radiant flux, in watts.
 * @param efficiency Luminous efficiency, on `[0, 1]`.
 * @return Luminous flux, in lumens.
 */
template <class T>
T watts_to_lumens(T radiant_flux, T efficiency)
{
	return radiant_flux * luminous_efficacy(efficiency);
}

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_PHOTOMETRY_HPP

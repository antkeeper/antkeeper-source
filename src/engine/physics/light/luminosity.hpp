// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_LIGHT_LUMINOSITY_HPP
#define ANTKEEPER_PHYSICS_LIGHT_LUMINOSITY_HPP

#include <cmath>

namespace physics {
namespace light {

/// Luminous efficiency functions.
namespace luminosity {

/// Fitted Gaussian approximation to the CIE 1931 standard observer photopic luminosity function.
/// @param lambda Wavelength of light, in nanometers.
/// @return Luminous efficiency, on `[0, 1]`.
/// @see Wyman, C., Sloan, P.J., & Shirley, P. (2013). Simple Analytic Approximations to the CIE XYZ Color Matching Functions.
template <class T>
T photopic(T lambda)
{
	const T t0 = (lambda - T(568.8)) * ((lambda < T(568.8)) ? T(0.0213) : T(0.0247));
	const T t1 = (lambda - T(530.9)) * ((lambda < T(530.9)) ? T(0.0613) : T(0.0322));
	
	const T y0 = T(0.821) * std::exp(T(-0.5) * t0 * t0);
	const T y1 = T(0.286) * std::exp(T(-0.5) * t1 * t1);
	
	return y0 + y1;
}

} // namespace luminosity

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_LUMINOSITY_HPP

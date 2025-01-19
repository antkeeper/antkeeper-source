// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_LIGHT_REFRACTION_HPP
#define ANTKEEPER_PHYSICS_LIGHT_REFRACTION_HPP

namespace physics {
namespace light {

/// Index of refraction formulas.
namespace ior {

/// Two-term form of Cauchy's transmission equation.
/// @param lambda Wavelength of light, in micrometers.
/// @param a First coefficient of the medium.
/// @param b Second coefficient of the medium.
/// @return Refractive index.
/// @see https://en.wikipedia.org/wiki/Cauchy%27s_equation
template <class T>
T cauchy(T lambda, T a, T b)
{
	return a + b / (lambda * lambda);
}

/// Three-term form of Cauchy's transmission equation.
/// @param lambda Wavelength of light, in micrometers.
/// @param a First coefficient.
/// @param b Second coefficient.
/// @param c Third coefficient.
/// @return Refractive index.
/// @see https://en.wikipedia.org/wiki/Cauchy%27s_equation
template <class T>
T cauchy(T lambda, T a, T b, T c)
{
	const T lambda2 = lambda * lambda;
	return a + b / lambda2 + c / (lambda2 * lambda2);
}

/// Two-term form of the Sellmeier equation.
/// @param lambda Wavelength of light, in micrometers.
/// @param b1 B1 coefficient.
/// @param c1 C1 coefficient.
/// @param b2 B2 coefficient.
/// @param c2 C2 coefficient.
/// @return Refractive index.
/// @see https://en.wikipedia.org/wiki/Sellmeier_equation
template <class T>
T sellmeier(T lambda, T b1, T c1, T b2, T c2)
{
	const T lambda2 = lambda * lambda;

	const T t1 = (b1 * lambda2) / (lambda2 - c1);
	const T t2 = (b2 * lambda2) / (lambda2 - c2);
	
	return std::sqrt(T(1) + t1 + t2);
}

/// Three-term form of the Sellmeier equation.
/// @param lambda Wavelength of light, in micrometers.
/// @param b1 B1 coefficient.
/// @param c1 C1 coefficient.
/// @param b2 B2 coefficient.
/// @param c2 C2 coefficient.
/// @param b3 B3 coefficient.
/// @param c3 C3 coefficient.
/// @return Refractive index.
/// @see https://en.wikipedia.org/wiki/Sellmeier_equation
template <class T>
T sellmeier(T lambda, T b1, T c1, T b2, T c2, T b3, T c3)
{
	const T lambda2 = lambda * lambda;

	const T t1 = (b1 * lambda2) / (lambda2 - c1);
	const T t2 = (b2 * lambda2) / (lambda2 - c2);
	const T t3 = (b3 * lambda2) / (lambda2 - c3);
	
	return std::sqrt(T(1) + t1 + t2 + t3);
}

} // namespace ior

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_REFRACTION_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_LIGHT_PHASE_HPP
#define ANTKEEPER_PHYSICS_LIGHT_PHASE_HPP

#include <engine/math/constants.hpp>
#include <cmath>

namespace physics {
namespace light {

/// Light-scattering phase functions.
namespace phase {

/**
 * Cornette-Shanks phase function.
 *
 * @param mu Cosine of the angle between the light and view directions.
 * @param g Asymmetry factor, on [-1, 1]. Positive values cause forward scattering, negative values cause back scattering.
 */
template <class T>
T cornette_shanks(T mu, T g);

/**
 * Henyey–Greenstein phase function.
 *
 * @param mu Cosine of the angle between the light and view directions.
 * @param g Asymmetry factor, on `[-1, 1]`. Positive values cause forward scattering, negative values cause back scattering.
 *
 * @see http://www.pbr-book.org/3ed-2018/Volume_Scattering/Phase_Functions.html
 */
template <class T>
T henyey_greenstein(T mu, T g);

/**
 * Isotropic phase function.
 */
template <class T>
constexpr T isotropic()
{
	return T(1) / (T(4) * math::pi<T>);
}

/**
 * Rayleigh phase function.
 *
 * @param mu Cosine of the angle between the light and view directions.
 */
template <class T>
T rayleigh(T mu);

template <class T>
T cornette_shanks(T mu, T g)
{
	static const T k = T(3) / (T(8) * math::pi<T>);
	const T gg = g * g;
	const T num = (T(1) - gg) * (T(1) + mu * mu);
	const T den = (T(2) + gg) * std::pow(T(1) + gg - T(2) * g * mu, T(1.5));
	return k * num / den;
}

template <class T>
T henyey_greenstein(T mu, T g)
{
	const T gg = g * g;
	return (T(1) - gg) / (T(4) * math::pi<T> * std::pow(T(1) + gg - T(2) * g * mu, T(1.5)));
}

template <class T>
T rayleigh(T mu)
{
	static const T k = T(3) / (T(16) * math::pi<T>);
	return k * (1.0 + mu * mu);
}

} // namespace phase

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_PHASE_HPP

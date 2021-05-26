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

#ifndef ANTKEEPER_PHYSICS_LIGHT_PHASE_HPP
#define ANTKEEPER_PHYSICS_LIGHT_PHASE_HPP

#include "math/constants.hpp"
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
 * Isotropic phase function. Causes light to be scattered uniformly in all directions.
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
	const T k = T(3) / (T(8) * math::pi<T>);
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
	return (T(1) + mu * mu) * (T(3) / (T(16) * math::pi<T>));
}

} // namespace phase

} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_PHASE_HPP

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

#ifndef ANTKEEPER_PHYSICS_CONSTANTS_HPP
#define ANTKEEPER_PHYSICS_CONSTANTS_HPP

namespace physics {

/// Physical constants
namespace constants {

/**
 * Boltzmann constant (k), in joule per kelvin.
 *
 * @see https://physics.nist.gov/cgi-bin/cuu/Value?k|search_for=universal_in!
 */
template <class T>
constexpr T boltzmann = T(1.380649e-23);

/**
 * Gravitational constant (G), in cubic meter per second squared per kilogram.
 *
 * @see https://physics.nist.gov/cgi-bin/cuu/Value?G|search_for=universal_in!
 */
template <class T>
constexpr T gravitational = T(6.67430e-11);

/**
 * Planck constant (h), in joule per hertz.
 *
 * @see https://physics.nist.gov/cgi-bin/cuu/Value?h|search_for=universal_in!
 */
template <class T>
constexpr T planck = T(6.62607015e-34);

/**
 * Stefan-Boltzmann constant (sigma), in watt per square meter kelvin to the fourth.
 *
 * @see https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_constant
 */
template <class T>
constexpr T stefan_boltzmann = T(5.67037441918442945397099673188923087584012297029130e-8);

/**
 * Speed of light in vacuum (c), in meters per second.
 *
 * @see https://physics.nist.gov/cgi-bin/cuu/Value?c|search_for=universal_in!
 */
template <class T>
constexpr T speed_of_light = T(299792458);

} // namespace constants

} // namespace physics

#endif // ANTKEEPER_PHYSICS_CONSTANTS_HPP

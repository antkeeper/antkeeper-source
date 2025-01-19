// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_NUMBER_DENSITY_HPP
#define ANTKEEPER_PHYSICS_NUMBER_DENSITY_HPP

#include <engine/physics/constants.hpp>

namespace physics {

/// Calculates the number density of a substance.
/// @param c Molar concentration, in mol/m-3.
/// @return Number density, in m-3.
/// @see https://en.wikipedia.org/wiki/Number_density
template <class T>
T number_density(T c)
{
	return physics::constants::avogadro<T> * c;
}

} // namespace physics

#endif // ANTKEEPER_PHYSICS_NUMBER_DENSITY_HPP

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

#ifndef ANTKEEPER_PHYSICS_TIME_UTC_HPP
#define ANTKEEPER_PHYSICS_TIME_UTC_HPP

#include <engine/math/numbers.hpp>

namespace physics {
namespace time {

/// Coordinated Universal Time (UTC).
namespace utc {

/**
 * Calculates the UTC offset at a given longitude
 *
 * @param longitude Longitude, in radians.
 * @return UTC offset.
 */
template <class T>
T offset(T longitude)
{
	return longitude / (math::two_pi<double> / 24.0);
}

} // namespace utc

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_UTC_HPP

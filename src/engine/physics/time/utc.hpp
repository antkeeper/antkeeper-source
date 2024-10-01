// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_TIME_UTC_HPP
#define ANTKEEPER_PHYSICS_TIME_UTC_HPP

#include <engine/math/constants.hpp>

namespace physics {
namespace time {

/// Coordinated Universal Time (UTC).
namespace utc {

/// Calculates the UTC offset at a given longitude
/// @param longitude Longitude, in radians.
/// @return UTC offset.
template <class T>
T offset(T longitude)
{
	return longitude / (math::two_pi<double> / 24.0);
}

} // namespace utc

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_UTC_HPP

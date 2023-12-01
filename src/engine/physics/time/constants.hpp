// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_TIME_CONSTANTS_HPP
#define ANTKEEPER_PHYSICS_TIME_CONSTANTS_HPP

namespace physics {
namespace time {

/// Number of seconds per day, in seconds.
template <class T>
constexpr T seconds_per_day = T(86400);

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_CONSTANTS_HPP

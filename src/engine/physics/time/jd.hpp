// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_TIME_JD_HPP
#define ANTKEEPER_PHYSICS_TIME_JD_HPP

namespace physics {
namespace time {

/// Julian day (JD).
namespace jd {

/// Number of Julian days per Julian century.
template <class T>
constexpr T days_per_century = T{36525};

/// Number of Julian centuries per Julian day.
template <class T>
constexpr T centuries_per_day = T{1} / T{36525};

/// Converts JD to UT1.
/// @param t JD time.
/// @return UT1 time.
template <class T>
T to_ut1(T t)
{
	return t - T(2451545);
}

} // namespace jd

} // namespace time
} // namespace physics

#endif // ANTKEEPER_PHYSICS_TIME_JD_HPP

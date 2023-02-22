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

#ifndef ANTKEEPER_SPRING_HPP
#define ANTKEEPER_SPRING_HPP

#include <engine/math/numbers.hpp>

/**
 * Contains the variables required for numeric springing.
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 *
 * @see spring()
 * @see solve_numeric_spring()
 */
template <typename T, typename S>
struct numeric_spring
{
	T x0; ///< Start value
	T x1; ///< End value
	T v;  ///< Velocity
	S z;  ///< Damping ratio, which can be undamped (z = 0), underdamped (z < 1), critically damped (z = 1), or overdamped (z > 1).
	S w;  ///< Angular frequency of the oscillation, in radians per second (2pi = 1Hz).
};

/**
 * Solves a number spring using the implicit Euler method.
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 *
 * @param[in,out] x0 Start value, which will be oscillated by this function.
 * @param[in,out] v Velocity, which will be modified by this function.
 * @param[in] x1 End value.
 * @param[in] z Damping ratio, which can be undamped (z = 0), underdamped (z < 1), critically damped (z = 1), or overdamped (z > 1).	
 * @param[in] w Angular frequency of the oscillation, in radians per second (2pi = 1Hz).
 * @param[in] dt Delta time, in seconds.
 */
template <typename T, typename S>
void spring(T& x0, T& v, const T& x1, S z, S w, S dt);

/**
 * Solves a number spring using the implicit Euler method.
 *
 * @param[in,out] ns Numeric spring to be sovled.
 * @param dt Delta time, in seconds.
 *
 * @see spring()
 */
template <typename T, typename S>
void solve_numeric_spring(numeric_spring<T, S>& ns, S dt);

/**
 * Converts a frequency from hertz to radians per second.
 *
 * @param hz Frequency in hertz.
 * @return Frequency in radians per second.
 */
template <typename T>
T hz_to_rads(T hz);

/**
 * Converts a frequency from radians per second to hertz.
 *
 * @param rads Frequency in radians per second.
 * @return Frequency in hertz.
 */
template <typename T>
T rads_to_hz(T rads);

/**
 * Converts a period from seconds to radians per second.
 *
 * @param t Period, in seconds.
 * @return Angular frequency, in radians per second.
 */
template <typename T>
T period_to_rads(T t);

template <typename T, typename S>
void spring(T& x0, T& v, const T& x1, S z, S w, S dt)
{
	const S ww_dt = w * w * dt;
	const S ww_dtdt = ww_dt * dt;
	const S f = z * w * dt * S{2} + S{1};
	const T det_x = x0 * f + v * dt + x1 * ww_dtdt;
	const T det_v = v + (x1 - x0) * ww_dt;
	const S inv_det = S{1} / (f + ww_dtdt);
	
	x0 = det_x * inv_det;
	v = det_v * inv_det;
}

template <typename T, typename S>
void solve_numeric_spring(numeric_spring<T, S>& ns, S dt)
{
	spring(ns.x0, ns.v, ns.x1, ns.z, ns.w, dt);
}

template <typename T>
inline T hz_to_rads(T hz)
{
	return hz * math::two_pi<T>;
}

template <typename T>
inline T rads_to_hz(T rads)
{
	return rads / math::two_pi<T>;
}

template <typename T>
inline T period_to_rads(T t)
{
	return math::two_pi<T> / t;
}

#endif // ANTKEEPER_SPRING_HPP

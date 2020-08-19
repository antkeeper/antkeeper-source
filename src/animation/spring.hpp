/*
 * Copyright (C) 2020  Christopher J. Howard
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

/**
 * Performs numeric, damped springing on a value and velocity.
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

template <typename T, typename S>
void spring(T& x0, T& v, const T& x1, S z, S w, S dt)
{
	const S w2_dt = w * w * dt;
	const S w2_dt2 = w2_dt * dt;
	const S f = z * w * dt * S(2) + S(1);
	const T det_x = x0 * f + v * dt + x1 * w2_dt2;
	const T det_v = v + (x1 - x0) * w2_dt;
	const S inv_det = S(1) / (f + w2_dt2);
	
	x0 = det_x * inv_det;
	v = det_v * inv_det;
}

#endif // ANTKEEPER_SPRING_HPP

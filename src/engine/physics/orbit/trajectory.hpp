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

#ifndef ANTKEEPER_PHYSICS_ORBIT_TRAJECTORY_HPP
#define ANTKEEPER_PHYSICS_ORBIT_TRAJECTORY_HPP

#include <engine/math/polynomial.hpp>
#include <engine/math/vector.hpp>
#include <vector>

namespace physics {
namespace orbit {

/**
 * Describes the trajectory of an orbit with Chebyshev polynomials.
 *
 * @tparam t Real type.
 */
template <class T>
struct trajectory
{
	/// Start time of the trajectory.
	T t0;
	
	/// End time of the trajectory.
	T t1;
	
	/// Time step duration.
	T dt;
	
	/// Chebyshev polynomial degree.
	std::size_t n;
	
	/// Chebyshev polynomial coefficients.
	std::vector<T> a;
	
	/**
	 * Calculates the Cartesian position of a trajectory at a given time.
	 *
	 * @param t Time, on `[t0, t1)`.
	 * @return Trajectory position at time @p t.
	 */
	[[nodiscard]] math::vector<T, 3> position(T t) const;
};

template <class T>
math::vector<T, 3> trajectory<T>::position(T t) const
{
	t -= t0;
	std::size_t i = static_cast<std::size_t>(t / dt);
	
	const T* ax = &a[i * n * 3];
	const T* ay = ax + n;
	const T* az = ay + n;
	
	t = (t / dt - i) * T(2) - T(1);
	
	math::vector3<T> r;
	r.x() = math::polynomial::chebyshev::evaluate(ax, ay, t);
	r.y() = math::polynomial::chebyshev::evaluate(ay, az, t);
	r.z() = math::polynomial::chebyshev::evaluate(az, az + n, t);
	
	return r;
}

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_TRAJECTORY_HPP

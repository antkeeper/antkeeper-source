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

#ifndef ANTKEEPER_PHYSICS_ORBIT_STATE_HPP
#define ANTKEEPER_PHYSICS_ORBIT_STATE_HPP

#include "utility/fundamental-types.hpp"

namespace physics {
namespace orbit {

/**
 * Pair of orbital state Cartesian position (r) and velocity (v) vectors.
 *
 * @tparam T Scalar type.
 */
template <class T>
struct state
{
	/// Scalar type.
	typedef T scalar_type;
	
	/// Vector type.
	typedef math::vector3<T> vector_type;
	
	/// Cartesian orbital position vector (r).
	vector_type r;
	
	/// Cartesian orbital velocity vector (v).
	vector_type v;
};

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_STATE_HPP

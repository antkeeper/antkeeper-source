// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_ORBIT_STATE_HPP
#define ANTKEEPER_PHYSICS_ORBIT_STATE_HPP

#include <engine/math/vector.hpp>

namespace physics {
namespace orbit {

/// Pair of orbital state Cartesian position (r) and velocity (v) vectors.
/// @tparam T Scalar type.
template <class T>
struct state
{
	/// Scalar type.
	using scalar_type = T;
	
	/// Vector type.
	using vector_type = math::vec3<T>;
	
	/// Cartesian orbital position vector (r).
	vector_type r;
	
	/// Cartesian orbital velocity vector (v).
	vector_type v;
};

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_STATE_HPP

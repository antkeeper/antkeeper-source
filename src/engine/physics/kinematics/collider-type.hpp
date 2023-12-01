// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_COLLIDER_TYPE_HPP
#define ANTKEEPER_PHYSICS_COLLIDER_TYPE_HPP

#include <cstdint>

namespace physics {

/**
 * Collider types.
 */
enum class collider_type: std::uint8_t
{
	/// Plane collider.
	plane,
	
	/// Sphere collider.
	sphere,
	
	/// Box collider.
	box,
	
	/// Capsule collider.
	capsule,
	
	/// Mesh collider.
	mesh
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_COLLIDER_TYPE_HPP

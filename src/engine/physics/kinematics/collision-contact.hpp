// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_COLLISION_CONTACT_HPP
#define ANTKEEPER_PHYSICS_COLLISION_CONTACT_HPP

#include <engine/math/vector.hpp>

namespace physics {

/**
 * Point of contact between two colliding bodies.
 */
struct collision_contact
{
	/// World-space contact point.
	math::fvec3 point{};
	
	/// Contact normal, pointing from body a to body b.
	math::fvec3 normal{};
	
	/// Contact penetration depth.
	float depth{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_COLLISION_CONTACT_HPP

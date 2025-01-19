// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_COLLISION_MANIFOLD_HPP
#define ANTKEEPER_PHYSICS_COLLISION_MANIFOLD_HPP

#include <engine/physics/kinematics/collision-contact.hpp>
#include <engine/physics/kinematics/rigid-body.hpp>
#include <array>
#include <cstdlib>

namespace physics {

/// Collection of contact points between two colliding bodies.
/// @tparam N Maximum number of contact points.
template <std::uint8_t N>
struct collision_manifold
{
	/// First colliding body.
	rigid_body* body_a{};
	
	/// Second colliding body.
	rigid_body* body_b{};
	
	/// Set of contact points between body a and body b.
	std::array<collision_contact, N> contacts;
	
	/// Number of contact points between body a and body b.
	std::uint8_t contact_count{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_COLLISION_MANIFOLD_HPP

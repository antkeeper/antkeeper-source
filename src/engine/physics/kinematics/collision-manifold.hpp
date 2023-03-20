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

#ifndef ANTKEEPER_PHYSICS_COLLISION_MANIFOLD_HPP
#define ANTKEEPER_PHYSICS_COLLISION_MANIFOLD_HPP

#include <engine/physics/kinematics/collision-contact.hpp>
#include <engine/physics/kinematics/rigid-body.hpp>
#include <array>
#include <cstdlib>

namespace physics {

/**
 * Collection of contact points between two colliding bodies.
 *
 * @param N Maximum number of contact points.
 */
template <std::uint8_t N>
struct collision_manifold
{
	/// First colliding body.
	rigid_body* body_a{nullptr};
	
	/// Second colliding body.
	rigid_body* body_b{nullptr};
	
	/// Set of contact points between body a and body b.
	std::array<collision_contact, N> contacts;
	
	/// Number of contact points between body a and body b.
	std::uint8_t contact_count{0};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_COLLISION_MANIFOLD_HPP

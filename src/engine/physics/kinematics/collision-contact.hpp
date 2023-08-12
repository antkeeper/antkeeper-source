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

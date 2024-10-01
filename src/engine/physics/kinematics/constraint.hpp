// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_CONSTRAINT_HPP
#define ANTKEEPER_PHYSICS_CONSTRAINT_HPP

namespace physics {

/// Abstract base class for rigid body constraints.
class constraint
{
public:
	/// Destructs a rigid body constraint.
	virtual ~constraint() = default;
	
	/// Solves the constraint.
	/// @param dt Timestep, in seconds.
	virtual void solve(float dt) = 0;
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_CONSTRAINT_HPP

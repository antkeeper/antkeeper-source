// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


namespace engine::physics
{
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
}

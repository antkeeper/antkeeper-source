// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


namespace engine::animation
{
	/// Abstract base class for IK solvers.
	class ik_solver
	{
	public:
		/// Destructs an IK solver.
		virtual ~ik_solver() = default;

		/// Transforms bones to find an inverse kinematic solution for an end effector.
		virtual void solve() = 0;
	};
}

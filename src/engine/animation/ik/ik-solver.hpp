// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_IK_SOLVER_HPP
#define ANTKEEPER_ANIMATION_IK_SOLVER_HPP

/// Abstract base class for IK solvers.
class ik_solver
{
public:
	/// Destructs an IK solver.
	virtual ~ik_solver() = default;
	
	/// Transforms bones to find an inverse kinematic solution for an end effector.
	virtual void solve() = 0;
};

#endif // ANTKEEPER_ANIMATION_IK_SOLVER_HPP

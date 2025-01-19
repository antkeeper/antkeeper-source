// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_THREE_DOF_CONSTRAINT_HPP
#define ANTKEEPER_GAME_THREE_DOF_CONSTRAINT_HPP

/// Builds rotation from 3DoF angles.
struct three_dof_constraint
{
	/// Yaw rotation angle, in radians.
	float yaw;
	
	/// Pitch rotation angle, in radians.
	float pitch;
	
	/// Roll rotation angle, in radians.
	float roll;
};

#endif // ANTKEEPER_GAME_THREE_DOF_CONSTRAINT_HPP

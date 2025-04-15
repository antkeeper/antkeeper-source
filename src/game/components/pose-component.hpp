// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_POSE_COMPONENT_HPP
#define ANTKEEPER_GAME_POSE_COMPONENT_HPP

import engine.animation.skeleton_pose;

using namespace engine;

struct pose_component
{
	/// Pose of the current state.
	animation::skeleton_pose current_pose;
	
	/// Pose of the previous state
	animation::skeleton_pose previous_pose;
};

#endif // ANTKEEPER_GAME_POSE_COMPONENT_HPP

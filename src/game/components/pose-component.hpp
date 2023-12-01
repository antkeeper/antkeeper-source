// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_POSE_COMPONENT_HPP
#define ANTKEEPER_GAME_POSE_COMPONENT_HPP

#include <engine/animation/pose.hpp>

/**
 * 
 */
struct pose_component
{
	/// Pose of the current state.
	pose current_pose;
	
	/// Pose of the previous state
	pose previous_pose;
};

#endif // ANTKEEPER_GAME_POSE_COMPONENT_HPP

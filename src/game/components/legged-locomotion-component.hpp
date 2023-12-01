// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_LEGGED_LOCOMOTION_COMPONENT_HPP
#define ANTKEEPER_GAME_LEGGED_LOCOMOTION_COMPONENT_HPP

#include <engine/math/vector.hpp>
#include <engine/animation/pose.hpp>
#include <engine/animation/locomotion/gait.hpp>
#include <memory>
#include <vector>

/**
 * Legged terrestrial locomotion.
 */
struct legged_locomotion_component
{
	/// Force vector.
	math::fvec3 force{0.0f, 0.0f, 0.0f};
	
	const pose* midstance_pose{};
	const pose* midswing_pose{};
	const pose* liftoff_pose{};
	const pose* touchdown_pose{};
	
	/// Indices of the the final bones in the legs.
	std::vector<bone_index_type> tip_bones;
	
	bone_index_type body_bone{};
	
	/// Number of bones per leg.
	std::uint8_t leg_bone_count{};
	
	std::shared_ptr<::gait> gait;
	
	float standing_height{};
	
	/// Distance covered in a single gait cycle.
	float stride_length{};
	
	/// Maximum angular frequency when turning, in radians per second.
	float max_angular_frequency{};
	
	float speed{};
	
	float angular_velocity{};
	
	math::fvec3 target_direction{};
	
	/// Current phase of the gait cycle, on `[0, 1]`.
	float gait_phase{};
};

#endif // ANTKEEPER_GAME_LEGGED_LOCOMOTION_COMPONENT_HPP

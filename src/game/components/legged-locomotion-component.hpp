// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_LEGGED_LOCOMOTION_COMPONENT_HPP
#define ANTKEEPER_GAME_LEGGED_LOCOMOTION_COMPONENT_HPP

#include <engine/animation/skeleton-pose.hpp>
#include <engine/animation/locomotion/gait.hpp>
#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>
#include <vector>

using namespace engine;

/// Legged terrestrial locomotion.
struct legged_locomotion_component
{
	/// Force vector.
	math::fvec3 force{0.0f, 0.0f, 0.0f};
	
	std::shared_ptr<animation::skeleton_pose> midstance_pose{};
	std::shared_ptr<animation::skeleton_pose> midswing_pose{};
	std::shared_ptr<animation::skeleton_pose> liftoff_pose{};
	std::shared_ptr<animation::skeleton_pose> touchdown_pose{};
	
	/// Indices of the the final bones in the legs.
	std::vector<usize> tip_bones;
	
	usize body_bone{};
	
	/// Number of bones per leg.
	u8 leg_bone_count{};
	
	std::shared_ptr<animation::gait> gait;
	
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

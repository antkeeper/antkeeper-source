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
	
	float speed{};
	
	float angular_velocity{};
	
	/// Current phase of the gait cycle, on `[0, 1]`.
	float gait_phase{};
};

#endif // ANTKEEPER_GAME_LEGGED_LOCOMOTION_COMPONENT_HPP

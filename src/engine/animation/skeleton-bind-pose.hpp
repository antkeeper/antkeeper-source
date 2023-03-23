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

#ifndef ANTKEEPER_ANIMATION_SKELETON_BIND_POSE_HPP
#define ANTKEEPER_ANIMATION_SKELETON_BIND_POSE_HPP

#include <engine/animation/skeleton-pose.hpp>

/**
 * Skeleton bind pose.
 */
class skeleton_bind_pose: public skeleton_pose
{
public:
	/**
	 * Constructs a skeleton bind pose.
	 *
	 * @param skeleton Skeleton with which to associate the bind pose.
	 */
	skeleton_bind_pose(const skeleton& skeleton);
	
	void update() override;
	void reset_bone_transforms() override;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_BIND_POSE_HPP

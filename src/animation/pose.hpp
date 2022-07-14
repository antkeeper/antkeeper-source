/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_ANIMATION_POSE_HPP
#define ANTKEEPER_ANIMATION_POSE_HPP

#include "animation/bone.hpp"
#include "math/transform-type.hpp"
#include <map>

/**
 * Skeletal animation pose.
 */
typedef std::map<bone, math::transform<float>, bone_index_compare> pose;

/**
 * Transforms a pose from bone-space into skeleton-space.
 *
 * @param[in] bone_space Bone-space pose.
 * @param[out] skeleton_space Skeleton-space pose.
 *
 * @warning If the index of any child bone is greater than its parent index, the concatenated pose may be incorrect.
 */
void concatenate(const pose& bone_space, pose& skeleton_space);

#endif // ANTKEEPER_ANIMATION_POSE_HPP

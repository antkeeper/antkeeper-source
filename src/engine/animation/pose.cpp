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

#include <engine/animation/pose.hpp>

void concatenate(const pose& bone_space, pose& skeleton_space)
{
	for (auto&& [bone, transform]: bone_space)
	{
		auto parent_index = bone_parent_index(bone);
		
		if (parent_index != bone_index(bone))
		{
			auto parent = skeleton_space.find(parent_index);
			skeleton_space[bone] = (parent != skeleton_space.end()) ? parent->second * transform : transform;
		}
		else
		{
			skeleton_space[bone] = transform;
		}
	}
}

void inverse(const pose& x, pose& y)
{
	for (auto&& [bone, transform]: x)
	{
		y[bone] = math::inverse(transform);
	}
}

void matrix_palette(const pose& inverse_bind_pose, const pose& pose, float4x4* palette)
{
	for (auto&& [bone, transform]: pose)
	{
		auto index = ::bone_index(bone);
		palette[index] = (inverse_bind_pose.at(bone) * transform).matrix();
	}
}

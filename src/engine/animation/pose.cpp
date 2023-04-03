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
#include <engine/animation/skeleton.hpp>
#include <algorithm>
#include <execution>

pose::pose(const skeleton& skeleton):
	m_skeleton(&skeleton),
	m_relative_transforms(skeleton.get_bone_count(), bone_transform_type::identity()),
	m_absolute_transforms(skeleton.get_bone_count(), bone_transform_type::identity())
{}

void pose::update()
{
	update(0, m_absolute_transforms.size());
}

void pose::update(bone_index_type first_index, std::size_t bone_count)
{
	bone_index_type child_index = first_index;
	
	std::for_each
	(
		std::execution::seq,
		m_absolute_transforms.begin() + first_index,
		m_absolute_transforms.begin() + (first_index + bone_count),
		[&](auto& child_absolute_transform)
		{
			const bone_index_type parent_index = m_skeleton->get_bone_parent(child_index);
			
			if (parent_index != child_index)
			{
				child_absolute_transform = m_absolute_transforms[parent_index] * m_relative_transforms[child_index];
			}
			else
			{
				child_absolute_transform = m_relative_transforms[child_index];
			}
			
			++child_index;
		}
	);
}

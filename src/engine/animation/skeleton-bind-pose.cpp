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

#include <engine/animation/skeleton-bind-pose.hpp>
#include <engine/animation/skeleton.hpp>

skeleton_bind_pose::skeleton_bind_pose(const skeleton& skeleton):
	skeleton_pose(skeleton)
{}

void skeleton_bind_pose::update()
{
	const std::size_t bone_count = m_relative_transforms.size();
	
	for (std::size_t i = 0; i < bone_count; ++i)
	{
		const bone_index_type parent_index = m_skeleton->get_bone_parent(static_cast<bone_index_type>(i));
		
		if (parent_index != i)
		{
			m_absolute_transforms[i] = m_absolute_transforms[parent_index] * m_relative_transforms[i];
		}
		else
		{
			m_absolute_transforms[i] = m_relative_transforms[i];
		}
		
		m_matrix_palette[i] = math::inverse(m_absolute_transforms[i]).matrix();
	}
}

void skeleton_bind_pose::reset_bone_transforms()
{
	const std::size_t bone_count = m_relative_transforms.size();
	for (std::size_t i = 0; i < bone_count; ++i)
	{
		m_relative_transforms[i] = bone_transform_type::identity();
		m_absolute_transforms[i] = bone_transform_type::identity();
		m_matrix_palette[i] = bone_matrix_type::identity();
	}
}

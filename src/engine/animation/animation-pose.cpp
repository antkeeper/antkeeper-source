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

#include <engine/animation/animation-pose.hpp>
#include <engine/animation/skeleton.hpp>
#include <algorithm>
#include <execution>

animation_pose::animation_pose(const skeleton& skeleton):
	pose(skeleton),
	m_matrix_palette(skeleton.get_bone_count())
{
	reset();
}

void animation_pose::update(bone_index_type first_index, std::size_t bone_count)
{
	// Update absolute transforms
	pose::update(first_index, bone_count);
	
	// Get skeleton rest pose
	const auto& rest_pose = m_skeleton->get_rest_pose();
	
	// Update skinning matrix palette
	std::for_each
	(
		std::execution::par_unseq,
		m_matrix_palette.begin() + first_index,
		m_matrix_palette.begin() + bone_count,
		[&](auto& skinning_matrix)
		{
			const bone_index_type bone_index = static_cast<bone_index_type>(&skinning_matrix - m_matrix_palette.data());
			skinning_matrix = (m_absolute_transforms[bone_index] * rest_pose.get_inverse_absolute_transform(bone_index)).matrix();
		}
	);
}

void animation_pose::reset()
{
	if (!m_skeleton)
	{
		return;
	}
	
	// Get skeleton rest pose
	const auto& rest_pose = m_skeleton->get_rest_pose();
	
	// Reset transforms and skinning matrix palette
	std::for_each
	(
		std::execution::par_unseq,
		m_relative_transforms.begin(),
		m_relative_transforms.end(),
		[&](auto& relative_transform)
		{
			const bone_index_type bone_index = static_cast<bone_index_type>(&relative_transform - m_relative_transforms.data());
			
			relative_transform = rest_pose.get_relative_transform(bone_index);
			m_absolute_transforms[bone_index] = rest_pose.get_absolute_transform(bone_index);
			m_matrix_palette[bone_index] = bone_matrix_type::identity();
		}
	);
}

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

#include <engine/animation/rest-pose.hpp>
#include <engine/animation/skeleton.hpp>
#include <algorithm>
#include <execution>

rest_pose::rest_pose(const skeleton& skeleton):
	pose(skeleton),
	m_inverse_absolute_transforms(skeleton.get_bone_count(), bone_transform_type::identity())
{}

void rest_pose::update(bone_index_type first_index, std::size_t bone_count)
{
	// Update absolute transforms
	pose::update(first_index, bone_count);
	
	// Update inverse absolute transforms
	std::for_each
	(
		std::execution::par_unseq,
		m_inverse_absolute_transforms.begin() + first_index,
		m_inverse_absolute_transforms.begin() + bone_count,
		[&](auto& inverse_absolute_transform)
		{
			bone_index_type bone_index = static_cast<bone_index_type>(&inverse_absolute_transform - m_inverse_absolute_transforms.data());
			inverse_absolute_transform = math::inverse(m_absolute_transforms[bone_index]);
		}
	);
}

void rest_pose::resize()
{
	if (m_skeleton)
	{
		const std::size_t bone_count = m_skeleton->get_bone_count();
		
		if (bone_count != m_inverse_absolute_transforms.size())
		{
			m_relative_transforms.resize(bone_count);
			m_absolute_transforms.resize(bone_count);
			m_inverse_absolute_transforms.resize(bone_count);
		}
	}
}

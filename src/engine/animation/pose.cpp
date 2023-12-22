// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/pose.hpp>
#include <engine/animation/skeleton.hpp>
#include <algorithm>
#include <execution>

pose::pose(const skeleton& skeleton):
	m_skeleton(&skeleton),
	m_relative_transforms(skeleton.get_bone_count(), math::identity<bone_transform_type>),
	m_absolute_transforms(skeleton.get_bone_count(), math::identity<bone_transform_type>)
{
	set_skeleton(skeleton);
}

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

void pose::set_skeleton(const skeleton& skeleton)
{
	m_skeleton = &skeleton;
	m_relative_transforms.resize(skeleton.get_bone_count(), math::identity<bone_transform_type>);
	m_absolute_transforms.resize(skeleton.get_bone_count(), math::identity<bone_transform_type>);
}

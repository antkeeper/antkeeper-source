// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
		// std::execution::par_unseq,
		std::execution::seq,
		m_matrix_palette.begin() + first_index,
		m_matrix_palette.begin() + (first_index + bone_count),
		[&](auto& skinning_matrix)
		{
			const bone_index_type bone_index = static_cast<bone_index_type>(&skinning_matrix - m_matrix_palette.data());
			skinning_matrix = (m_absolute_transforms[bone_index] * rest_pose.get_inverse_absolute_transform(bone_index)).matrix();
		}
	);
}

void animation_pose::set_skeleton(const skeleton& skeleton)
{
	pose::set_skeleton(skeleton);
	m_matrix_palette.resize(skeleton.get_bone_count(), math::identity<bone_matrix_type>);
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
		// std::execution::par_unseq,
		std::execution::seq,
		m_relative_transforms.begin(),
		m_relative_transforms.end(),
		[&](auto& relative_transform)
		{
			const bone_index_type bone_index = static_cast<bone_index_type>(&relative_transform - m_relative_transforms.data());
			
			relative_transform = rest_pose.get_relative_transform(bone_index);
			m_absolute_transforms[bone_index] = rest_pose.get_absolute_transform(bone_index);
			m_matrix_palette[bone_index] = math::identity<bone_matrix_type>;
		}
	);
}

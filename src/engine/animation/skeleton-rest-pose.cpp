// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.animation.skeleton_rest_pose;
import engine.animation.skeleton;
import engine.utility.sized_types;

namespace engine::animation
{
	skeleton_rest_pose::skeleton_rest_pose(animation::skeleton& skeleton):
		skeleton_pose(skeleton, true /* is_rest_pose */),
		m_inverse_absolute_transforms(skeleton.bones().size(), math::identity<math::transform<float>>)
	{
	}

	void skeleton_rest_pose::update() const
	{
		skeleton_pose::update();

		update_inverse_absolute_transforms();
	}

	void skeleton_rest_pose::reset()
	{
		std::fill(m_relative_transforms.begin(), m_relative_transforms.end(), math::identity<math::transform<float>>);
		std::fill(m_absolute_transforms.begin(), m_absolute_transforms.end(), math::identity<math::transform<float>>);
		std::fill(m_inverse_absolute_transforms.begin(), m_inverse_absolute_transforms.end(), math::identity<math::transform<float>>);
		std::fill(m_skinning_matrices.begin(), m_skinning_matrices.end(), math::identity<math::fmat4>);
		std::fill(m_bone_flags.begin(), m_bone_flags.end(), u8{0});
	}

	void skeleton_rest_pose::update_inverse_absolute_transforms() const
	{
		for (usize i = 0; i < m_inverse_absolute_transforms.size(); ++i)
		{
			if (is_inverse_absolute_transform_outdated(i))
			{
				update_inverse_absolute_transform(i);
			}
		}
	}

	const math::transform<float>& skeleton_rest_pose::get_inverse_absolute_transform(usize index) const
	{
		if (is_inverse_absolute_transform_outdated(index))
		{
			update_inverse_absolute_transform(index);
		}

		return m_inverse_absolute_transforms[index];
	}

	const std::vector<math::transform<float>>& skeleton_rest_pose::get_inverse_absolute_transforms() const
	{
		update_inverse_absolute_transforms();

		return m_inverse_absolute_transforms;
	}

	void skeleton_rest_pose::update_skinning_matrix(usize index) const
	{
		// Rest post skinning matrix is always identity, no need to update

		// Clear skinning matrix outdated flag
		m_bone_flags[index] &= ~skinning_matrix_outdated_flag;
	}

	void skeleton_rest_pose::update_inverse_absolute_transform(usize index) const
	{
		// Update inverse absolute transform
		m_inverse_absolute_transforms[index] = math::inverse(get_absolute_transform(index));

		// Clear inverse absolute transform outdated flag
		m_bone_flags[index] &= ~inverse_absolute_transform_outdated_flag;
	}
}

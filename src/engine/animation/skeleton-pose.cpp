// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.animation.skeleton_pose;
import engine.animation.skeleton;
import engine.utility.sized_types;

namespace engine::animation
{
	skeleton_pose::skeleton_pose(animation::skeleton& skeleton):
		m_skeleton{&skeleton},
		m_relative_transforms(skeleton.rest_pose().get_relative_transforms()),
		m_absolute_transforms(skeleton.rest_pose().get_absolute_transforms()),
		m_skinning_matrices(skeleton.bones().size(), math::identity<math::fmat4>),
		m_bone_flags(skeleton.bones().size(), 0)
	{
	}

	skeleton_pose::skeleton_pose(animation::skeleton& skeleton, bool /* is_rest_pose */):
		m_skeleton{&skeleton},
		m_relative_transforms(skeleton.bones().size(), math::identity<math::transform<float>>),
		m_absolute_transforms(skeleton.bones().size(), math::identity<math::transform<float>>),
		m_skinning_matrices(skeleton.bones().size(), math::identity<math::fmat4>),
		m_bone_flags(skeleton.bones().size(), 0)
	{
	}

	void skeleton_pose::update_absolute_transforms() const
	{
		for (usize i = 0; i < m_absolute_transforms.size(); ++i)
		{
			if (is_absolute_transform_outdated(i))
			{
				update_absolute_transform(i);
			}
		}
	}

	void skeleton_pose::update_skinning_matrices() const
	{
		for (usize i = 0; i < m_skinning_matrices.size(); ++i)
		{
			if (is_skinning_matrix_outdated(i))
			{
				update_skinning_matrix(i);
			}
		}
	}

	void skeleton_pose::update() const
	{
		// Implicitly updates absolute transforms
		update_skinning_matrices();
	}

	void skeleton_pose::reset()
	{
		// Get relative and absolute transforms of rest pose (automatically updating where outdated)
		const auto& rest_relative_transforms = m_skeleton->rest_pose().get_relative_transforms();
		const auto& rest_absolute_transforms = m_skeleton->rest_pose().get_absolute_transforms();

		// Set relative and absolute transforms of this pose to that of rest pose
		m_relative_transforms.assign(rest_relative_transforms.begin(), rest_relative_transforms.end());
		m_absolute_transforms.assign(rest_absolute_transforms.begin(), rest_absolute_transforms.end());

		// Fill skinning matrices with identity matrix (same as rest pose)
		std::fill(m_skinning_matrices.begin(), m_skinning_matrices.end(), math::identity<math::fmat4>);

		// Clear all bone flags
		std::fill(m_bone_flags.begin(), m_bone_flags.end(), u8{0});
	}

	void skeleton_pose::set_relative_transform(usize index, const math::transform<float>& transform)
	{
		m_relative_transforms[index] = transform;

		if (!is_absolute_transform_outdated(index))
		{
			flag_bones(index, absolute_transform_outdated_flag | inverse_absolute_transform_outdated_flag | skinning_matrix_outdated_flag);
		}
	}

	void skeleton_pose::set_relative_translation(usize index, const math::fvec3& translation)
	{
		m_relative_transforms[index].translation = translation;

		if (!is_absolute_transform_outdated(index))
		{
			flag_bones(index, absolute_transform_outdated_flag | inverse_absolute_transform_outdated_flag | skinning_matrix_outdated_flag);
		}
	}

	void skeleton_pose::set_relative_rotation(usize index, const math::fquat& rotation)
	{
		m_relative_transforms[index].rotation = rotation;

		if (!is_absolute_transform_outdated(index))
		{
			flag_bones(index, absolute_transform_outdated_flag | inverse_absolute_transform_outdated_flag | skinning_matrix_outdated_flag);
		}
	}

	void skeleton_pose::set_relative_scale(usize index, const math::fvec3& scale)
	{
		m_relative_transforms[index].scale = scale;

		if (!is_absolute_transform_outdated(index))
		{
			flag_bones(index, absolute_transform_outdated_flag | inverse_absolute_transform_outdated_flag | skinning_matrix_outdated_flag);
		}
	}

	void skeleton_pose::set_absolute_transform(usize index, const math::transform<float>& transform)
	{
		// Set absolute transform
		m_absolute_transforms[index] = transform;

		// Calculate relative transform
		if (auto parent = m_skeleton->bones()[index].parent())
		{
			m_relative_transforms[index] = math::inverse(get_absolute_transform(parent->index())) * transform;
		}
		else
		{
			m_relative_transforms[index] = transform;
		}

		// Flag bone and descendants as outdated
		if (!is_absolute_transform_outdated(index))
		{
			flag_bones(index, absolute_transform_outdated_flag | inverse_absolute_transform_outdated_flag | skinning_matrix_outdated_flag);
		}

		// Clear absolute transform outdated flag
		m_bone_flags[index] &= ~absolute_transform_outdated_flag;
	}

	const math::transform<float>& skeleton_pose::get_absolute_transform(usize index) const
	{
		if (is_absolute_transform_outdated(index))
		{
			update_absolute_transform(index);
		}

		return m_absolute_transforms[index];
	}

	const std::vector<math::transform<float>>& skeleton_pose::get_absolute_transforms() const
	{
		update_absolute_transforms();

		return m_absolute_transforms;
	}

	const math::fmat4& skeleton_pose::get_skinning_matrix(usize index) const
	{
		if (is_skinning_matrix_outdated(index))
		{
			update_skinning_matrix(index);
		}

		return m_skinning_matrices[index];
	}

	const std::vector<math::fmat4>& skeleton_pose::get_skinning_matrices() const
	{
		update_skinning_matrices();

		return m_skinning_matrices;
	}

	void skeleton_pose::flag_bones(usize index, u8 flags)
	{
		// Add bone to traversal
		m_bone_traversal.emplace_back(&m_skeleton->bones()[index]);

		// Traverse bones
		do
		{
			// Get current bone
			const auto current = m_bone_traversal.back();

			// Flag current bone
			m_bone_flags[current->index()] |= flags;

			// Remove current bone from traversal
			m_bone_traversal.pop_back();

			// Add child bones to traversal
			for (const auto& child : current->children())
			{
				m_bone_traversal.emplace_back(child);
			}
		} while (!m_bone_traversal.empty());
	}

	bool skeleton_pose::is_absolute_transform_outdated(usize index) const noexcept
	{
		return m_bone_flags[index] & absolute_transform_outdated_flag;
	}

	bool skeleton_pose::is_inverse_absolute_transform_outdated(usize index) const noexcept
	{
		return m_bone_flags[index] & inverse_absolute_transform_outdated_flag;
	}

	bool skeleton_pose::is_skinning_matrix_outdated(usize index) const noexcept
	{
		return m_bone_flags[index] & skinning_matrix_outdated_flag;
	}

	void skeleton_pose::update_absolute_transform(usize index) const
	{
		const auto& bone = m_skeleton->bones()[index];

		if (bone.parent())
		{
			// Build update traversal
			for (auto current = bone.parent(); current && m_bone_flags[current->index()]; current = current->parent())
			{
				m_bone_traversal.emplace_back(current);
			}

			// Traverse flagged ancestors
			while (!m_bone_traversal.empty())
			{
				const auto current = m_bone_traversal.back();

				// Update absolute transform
				if (current->parent())
				{
					m_absolute_transforms[current->index()] = m_absolute_transforms[current->parent()->index()] * m_relative_transforms[current->index()];
				}
				else
				{
					m_absolute_transforms[current->index()] = m_relative_transforms[current->index()];
				}

				// Clear absolute transform outdated flag
				m_bone_flags[current->index()] &= ~absolute_transform_outdated_flag;

				// Remove bone from traversal
				m_bone_traversal.pop_back();
			}

			m_absolute_transforms[index] = m_absolute_transforms[bone.parent()->index()] * m_relative_transforms[index];
		}
		else
		{
			m_absolute_transforms[index] = m_relative_transforms[index];
		}

		// Clear absolute transform outdated flag
		m_bone_flags[index] &= ~absolute_transform_outdated_flag;
	}

	void skeleton_pose::update_skinning_matrix(usize index) const
	{
		// Update skinning matrix
		m_skinning_matrices[index] = (get_absolute_transform(index) * m_skeleton->rest_pose().get_inverse_absolute_transform(index)).matrix();

		// Clear skinning matrix outdated flag
		m_bone_flags[index] &= ~skinning_matrix_outdated_flag;
	}
}

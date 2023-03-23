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

#ifndef ANTKEEPER_ANIMATION_SKELETON_POSE_HPP
#define ANTKEEPER_ANIMATION_SKELETON_POSE_HPP

#include <engine/math/transform.hpp>
#include <engine/math/matrix.hpp>
#include <cstdint>
#include <vector>

class skeleton;

/**
 * Skeleton pose.
 */
class skeleton_pose
{
public:
	/// Bone index type.
	using bone_index_type = std::uint16_t;
	
	/// Bone transform type.
	using bone_transform_type = math::transform<float>;
	
	/// Matrix type.
	using bone_matrix_type = math::matrix<float, 4, 4>;
	
	/**
	 * Constructs a skeleton pose.
	 *
	 * @param skeleton Skeleton with which to associate the pose.
	 */
	explicit skeleton_pose(const skeleton& skeleton);
	
	/// Constructs an empty skeleton pose.
	skeleton_pose() noexcept = default;
	
	/**
	 * Updates the absolute transforms and matrix palette of the pose.
	 */
	virtual void update();
	
	/**
	 * Resets all bone transforms to identity transforms.
	 */
	virtual void reset_bone_transforms();
	
	/**
	 * Sets the number of bones in the pose.
	 *
	 * @param skeleton Skeleton with which to associate the pose.
	 */
	void set_skeleton(const skeleton* skeleton);
	
	/**
	 * Sets the relative transform describing a bone pose.
	 *
	 * @param index Index of a bone.
	 * @param transform Relative transform describing the bone pose.
	 */
	inline void set_relative_transform(bone_index_type index, const bone_transform_type& transform)
	{
		m_relative_transforms[index] = transform;
	}
	
	/// Returns the skeleton with which the pose is associated.
	[[nodiscard]] inline const skeleton* get_skeleton() const noexcept
	{
		return m_skeleton;
	}
	
	/**
	 * Returns the relative transform describing a bone pose.
	 *
	 * @param index Index of a bone.
	 *
	 * @return Relative transform describing the bone pose.
	 */
	[[nodiscard]] inline const bone_transform_type& get_relative_transform(bone_index_type index) const
	{
		return m_relative_transforms[index];
	}
	
	/**
	 * Returns the absolute transform describing a bone pose.
	 *
	 * @param index Index of a bone.
	 *
	 * @return Absolute transform describing the bone pose.
	 */
	[[nodiscard]] inline const bone_transform_type& get_absolute_transform(bone_index_type index) const
	{
		return m_absolute_transforms[index];
	}
	
	/**
	 * Returns the matrix palette of the pose.
	 *
	 * @note The matrix palette of a standard skeleton pose will contain its skinning matrices.
	 * @note The matrix palette of a skeleton bind pose will contain the inverses of its absolute transforms.
	 */
	[[nodiscard]] inline const std::vector<bone_matrix_type>& get_matrix_palette() const noexcept
	{
		return m_matrix_palette;
	}
	
protected:
	/// Skeleton with which the pose is associated.
	const skeleton* m_skeleton{nullptr};
	
	/// Relative ransforms for each bone in a skeleton.
	std::vector<bone_transform_type> m_relative_transforms;
	
	/// Absolute transforms for each bone in a skeleton.
	std::vector<bone_transform_type> m_absolute_transforms;
	
	/// Skinning matrix palette.
	std::vector<bone_matrix_type> m_matrix_palette;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_POSE_HPP

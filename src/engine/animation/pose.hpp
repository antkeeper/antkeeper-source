// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_POSE_HPP
#define ANTKEEPER_ANIMATION_POSE_HPP

#include <engine/animation/bone.hpp>
#include <vector>

class skeleton;

/**
 * Base class for skeleton poses.
 */
class pose
{
public:
	/**
	 * Constructs a pose.
	 *
	 * @param skeleton Skeleton with which to associate the pose.
	 */
	explicit pose(const skeleton& skeleton);
	
	/// Constructs an empty pose.
	constexpr pose() noexcept = default;
	
	/**
	 * Updates the pose after one or more relative transforms have been changed.
	 */
	void update();
	
	/**
	 * Updates a subset of the pose after one or more relative transforms have been changed.
	 *
	 * @param first_index Index of the first bone in the chain to update.
	 * @param bone_count Number of bones in the chain to update.
	 *
	 * @warning It's the caller's responsibility to ensure that any ancestors of the bone chain are up to date before the call, and any descendants are updated after the call.
	 */
	virtual void update(bone_index_type first_index, std::size_t bone_count);
	
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
	
	/**
	 * Sets the relative translation of a bone pose.
	 *
	 * @param index Index of a bone.
	 * @param translation Relative translation of the bone pose.
	 */
	inline void set_relative_translation(bone_index_type index, const bone_transform_type::vector_type& translation)
	{
		m_relative_transforms[index].translation = translation;
	}
	
	/**
	 * Sets the relative rotation of a bone pose.
	 *
	 * @param index Index of a bone.
	 * @param translation Relative rotation of the bone pose.
	 */
	inline void set_relative_rotation(bone_index_type index, const bone_transform_type::quaternion_type& rotation)
	{
		m_relative_transforms[index].rotation = rotation;
	}
	
	/**
	 * Sets the relative scale of a bone pose.
	 *
	 * @param index Index of a bone.
	 * @param scale Relative scale of the bone pose.
	 */
	inline void set_relative_scale(bone_index_type index, const bone_transform_type::vector_type& scale)
	{
		m_relative_transforms[index].scale = scale;
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
	
protected:
	/// Skeleton with which the pose is associated.
	const skeleton* m_skeleton{nullptr};
	
	/// Relative ransforms for each bone in a skeleton.
	std::vector<bone_transform_type> m_relative_transforms;
	
	/// Absolute transforms for each bone in a skeleton.
	std::vector<bone_transform_type> m_absolute_transforms;
};

#endif // ANTKEEPER_ANIMATION_POSE_HPP

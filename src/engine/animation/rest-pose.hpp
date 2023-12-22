// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_REST_POSE_HPP
#define ANTKEEPER_ANIMATION_REST_POSE_HPP

#include <engine/animation/pose.hpp>

/**
 * Skeleton rest pose.
 */
class rest_pose: public pose
{
public:
	/**
	 * Constructs a rest pose.
	 *
	 * @param skeleton Skeleton with which to associate the rest pose.
	 */
	explicit rest_pose(const skeleton& skeleton);
	
	/** Constructs an empty rest pose. */
	rest_pose() noexcept = default;
	
	/** Destructs a rest pose. */
	~rest_pose() override = default;
	
	using pose::update;
	void update(bone_index_type first_index, std::size_t bone_count) override;
	
	/**
	 * Associates this rese pose with a skeleton.
	 *
	 * @param skeleton Skeleton with which to associate the rest pose.
	 */
	void set_skeleton(const skeleton& skeleton) override;
	
	/**
	 * Updates the number of bones in the rest pose if the skeleton has been modified.
	 */
	void resize();
	
	/**
	 * Returns the inverse of the absolute transform describing a bone pose.
	 *
	 * @param index Index of a bone.
	 *
	 * @return Inverse of the absolute transform describing the bone pose.
	 */
	[[nodiscard]] inline const bone_transform_type& get_inverse_absolute_transform(bone_index_type index) const
	{
		return m_inverse_absolute_transforms[index];
	}
	
private:
	/// Inverse absolute transforms for each bone in the associated skeleton.
	std::vector<bone_transform_type> m_inverse_absolute_transforms;
};

#endif // ANTKEEPER_ANIMATION_REST_POSE_HPP

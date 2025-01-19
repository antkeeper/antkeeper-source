// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_SKELETON_REST_POSE_HPP
#define ANTKEEPER_ANIMATION_SKELETON_REST_POSE_HPP

#include <engine/animation/skeleton-pose.hpp>

/// Skeleton pose with inverse absolute transforms for skinning matrix calculation.
class skeleton_rest_pose: public skeleton_pose
{
public:
	/// Constructs a skeleton rest pose.
	/// @param skeleton Skeleton with which to associate the rest pose.
	explicit skeleton_rest_pose(::skeleton& skeleton);
	
	/// Constructs an empty skeleton rest pose.
	skeleton_rest_pose() noexcept = default;

	/// Copy-constructs a skeleton rest pose.
	skeleton_rest_pose(const skeleton_rest_pose&) = default;

	/// Move-constructs a skeleton rest pose.
	skeleton_rest_pose(skeleton_rest_pose&&) = default;
	
	/// Destructs a skeleton rest pose.
	~skeleton_rest_pose() override = default;

	/// Copy-assigns a skeleton rest pose.
	skeleton_rest_pose& operator=(const skeleton_rest_pose&) = default;

	/// Move-assigns a skeleton rest pose.
	skeleton_rest_pose& operator=(skeleton_rest_pose&&) = default;

	/// Explicitly updates all outdated transforms and skinning matrices in the pose.
	void update() const override;

	/// Resets all transforms to identity.
	void reset() override;

	/// Explicitly updates all outdated inverse absolute transforms in the pose.
	void update_inverse_absolute_transforms() const;

	/// Returns the inverse absolute transform describing a bone pose.
	/// @param index Index of a bone.
	/// @return Inverse absolute transform describing the bone pose.
	/// @note Automatically updates the inverse absolute transform of the bone, if outdated, as well as the absolute transforms of the bone and its ancestors, where outdated.
	[[nodiscard]] const math::transform<float>& get_inverse_absolute_transform(std::size_t index) const;

	/// Returns the inverse absolute transforms of the skeleton pose.
	/// @note Automatically updates all outdated inverse absolute transforms.
	[[nodiscard]] const std::vector<math::transform<float>>& get_inverse_absolute_transforms() const;
	
private:
	/// Updates the skinning matrix of a bone pose.
	/// @note Rest pose skinning matrices are always identity.
	void update_skinning_matrix(std::size_t index) const override;

	/// Updates the inverse absolute transform of a bone pose.
	/// @param index Index of a bone.
	/// @note Additionally updates the absolute transform of the bone and its ancestors, where outdated.
	void update_inverse_absolute_transform(std::size_t index) const;

	mutable std::vector<math::transform<float>> m_inverse_absolute_transforms;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_REST_POSE_HPP

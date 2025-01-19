// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_SKELETON_POSE_HPP
#define ANTKEEPER_ANIMATION_SKELETON_POSE_HPP

#include <engine/animation/bone.hpp>
#include <engine/math/transform.hpp>
#include <engine/math/matrix.hpp>
#include <cstdint>
#include <vector>

class skeleton;
class bone;

/// Skeleton pose.
class skeleton_pose
{
public:
	/// Constructs a skeleton pose.
	/// @details All transforms will be initialized to the skeleton rest pose transforms.
	/// @param skeleton Skeleton with which to associate the pose.
	explicit skeleton_pose(::skeleton& skeleton);
	
	/// Constructs an empty skeleton pose.
	skeleton_pose() noexcept = default;

	/// Copy-constructs a skeleton pose.
	skeleton_pose(const skeleton_pose& other) = default;

	/// Move-constructs a skeleton pose.
	skeleton_pose(skeleton_pose&& other) = default;
	
	/// Destructs a skeleton pose.
	virtual ~skeleton_pose() = default;

	/// Copy-assigns a skeleton pose.
	skeleton_pose& operator=(const skeleton_pose& other) = default;

	/// Move-assigns a skeleton pose.
	skeleton_pose& operator=(skeleton_pose&& other) = default;

	/// Explicitly updates all outdated absolute transforms in the pose.
	void update_absolute_transforms() const;

	/// Explicitly updates all outdated skinning matrices in the pose.
	/// @note Consequently updates all outdated absolute transforms in the pose.
	void update_skinning_matrices() const;

	/// Explicitly updates all outdated transforms and skinning matrices in the pose.
	virtual void update() const;

	/// Resets the pose to the rest pose.
	virtual void reset();
	
	/// Sets the relative transform describing a bone pose.
	/// @param index Index of a bone.
	/// @param transform Relative transform describing the bone pose.
	void set_relative_transform(std::size_t index, const math::transform<float>& transform);
	
	/// Sets the relative translation of a bone pose.
	/// @param index Index of a bone.
	/// @param translation Relative translation of the bone pose.
	void set_relative_translation(std::size_t index, const math::transform<float>::vector_type& translation);
	
	/// Sets the relative rotation of a bone pose.
	/// @param index Index of a bone.
	/// @param rotation Relative rotation of the bone pose.
	void set_relative_rotation(std::size_t index, const math::transform<float>::quaternion_type& rotation);
	
	/// Sets the relative scale of a bone pose.
	/// @param index Index of a bone.
	/// @param scale Relative scale of the bone pose.
	void set_relative_scale(std::size_t index, const math::transform<float>::vector_type& scale);

	/// Sets the absolute transform describing a bone pose.
	/// @param index Index of a bone.
	/// @param transform Absolute transform describing the bone pose.
	void set_absolute_transform(std::size_t index, const math::transform<float>& transform);
	
	/// Returns the relative transform describing a bone pose.
	/// @param index Index of a bone.
	/// @return Relative transform describing the bone pose.
	[[nodiscard]] inline const math::transform<float>& get_relative_transform(std::size_t index) const
	{
		return m_relative_transforms[index];
	}

	/// Returns the relative transforms of the skeleton pose.
	[[nodiscard]] inline const std::vector<math::transform<float>>& get_relative_transforms() const
	{
		return m_relative_transforms;
	}
	
	/// Returns the absolute transform describing a bone pose.
	/// @param index Index of a bone.
	/// @return Absolute transform describing the bone pose.
	/// @note Automatically updates the absolute transforms of the bone and its ancestors, where outdated.
	[[nodiscard]] const math::transform<float>& get_absolute_transform(std::size_t index) const;

	/// Returns the absolute transforms of the skeleton pose.
	/// @note Automatically updates absolute transforms, where outdated.
	[[nodiscard]] const std::vector<math::transform<float>>& get_absolute_transforms() const;

	/// Returns the skinning matrix of a bone pose.
	/// @param index Index of a bone.
	/// @return Skinning matrix of the bone pose.
	/// @note Automatically updates the skinning matrix of the bone, if outdated, as well as the absolute transforms of the bone and its ancestors, where outdated.
	[[nodiscard]] const math::fmat4& get_skinning_matrix(std::size_t index) const;

	/// Returns the skinning matrices of the skeleton pose.
	/// @note Automatically updates skinning matrices and absolute transforms, where outdated.
	[[nodiscard]] const std::vector<math::fmat4>& get_skinning_matrices() const;

	/// Returns the skeleton with which the pose is associated.
	[[nodiscard]] inline skeleton* get_skeleton() noexcept
	{
		return m_skeleton;
	}

	/// @copydoc get_skeleton()
	[[nodiscard]] inline const skeleton* get_skeleton() const noexcept
	{
		return m_skeleton;
	}
	
protected:
	friend class skeleton;

	/// @private
	enum bone_flags: std::uint8_t
	{
		/// Absolute transform outdated.
		absolute_transform_outdated_flag = 0b01,

		/// Inverse absolute transform outdated.
		inverse_absolute_transform_outdated_flag = 0b10,

		/// Skinning matrix outdated.
		skinning_matrix_outdated_flag    = 0b100
	};

	/// Constructs a skeleton pose from a skeleton rest pose.
	/// @details All transforms will be intialized to identity transforms.
	/// @param skeleton Skeleton with which to associate the pose.
	/// @param is_rest_pose Unused parameter to indicate the pose is a rest pose, regardless of value.
	skeleton_pose(::skeleton& skeleton, bool is_rest_pose);

	/// Flags a bone and all of its descendants.
	/// @param index Index of the first bone to flag.
	/// @param flags Bone flags.
	void flag_bones(std::size_t index, std::uint8_t flags);

	/// Checks if the absolute transform for a bone pose is outdated.
	/// @param index Index of a bone.
	/// @return `true` if the absolute transform for the given bone pose is outdated, `false` otherwise.
	[[nodiscard]] bool is_absolute_transform_outdated(std::size_t index) const noexcept;

	/// Checks if the inverse absolute transform for a bone pose is outdated.
	/// @param index Index of a bone.
	/// @return `true` if the inverse absolute transform for the given bone pose is outdated, `false` otherwise.
	[[nodiscard]] bool is_inverse_absolute_transform_outdated(std::size_t index) const noexcept;

	/// Checks if the skinning matrix for a bone pose is outdated.
	/// @param index Index of a bone.
	/// @return `true` if the skinning matrix for the given bone pose is outdated, `false` otherwise.
	[[nodiscard]] bool is_skinning_matrix_outdated(std::size_t index) const noexcept;

	/// Updates the absolute transform of a bone and all of its outdated ancestors.
	/// @param index Index of a bone.
	void update_absolute_transform(std::size_t index) const;

	/// Updates the absolute skinning matrix of a bone.
	/// @param index Index of a bone.
	/// @note Additionally updates the absolute transform of the bone and its ancestors, where outdated.
	virtual void update_skinning_matrix(std::size_t index) const;

	skeleton* m_skeleton{nullptr};
	std::vector<math::transform<float>> m_relative_transforms;
	mutable std::vector<math::transform<float>> m_absolute_transforms;
	mutable std::vector<math::fmat4> m_skinning_matrices;
	mutable std::vector<std::uint8_t> m_bone_flags;
	mutable std::vector<const bone*> m_bone_traversal;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_POSE_HPP

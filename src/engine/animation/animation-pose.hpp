// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_POSE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_POSE_HPP

#include <engine/animation/pose.hpp>
#include <vector>

/**
 * Animatable skeleton pose.
 */
class animation_pose: public pose
{
public:
	/**
	 * Constructs an animation pose.
	 *
	 * @param skeleton Skeleton with which to associate the animation pose.
	 */
	explicit animation_pose(const skeleton& skeleton);
	
	/** Constructs an empty animation pose. */
	animation_pose() noexcept = default;
	
	/** Destructs an animation pose. */
	~animation_pose() override = default;
	
	using pose::update;
	void update(bone_index_type first_index, std::size_t bone_count) override;
	
	/**
	 * Associates this animation pose with a skeleton.
	 *
	 * @param skeleton Skeleton with which to associate the animation pose.
	 */
	void set_skeleton(const skeleton& skeleton) override;
	
	/**
	 * Resets the animation pose to the skeleton's rest pose.
	 */
	void reset();
	
	/**
	 * Returns the skinning matrix palette of the animation pose.
	 */
	[[nodiscard]] inline const std::vector<bone_matrix_type>& get_matrix_palette() const noexcept
	{
		return m_matrix_palette;
	}
	
private:
	/// Skinning matrix palette.
	std::vector<bone_matrix_type> m_matrix_palette;
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_POSE_HPP

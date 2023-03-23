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
	
	/// Constructs an empty animation pose.
	animation_pose() noexcept = default;
	
	using pose::update;
	void update(bone_index_type first_index, std::size_t bone_count) override;
	
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

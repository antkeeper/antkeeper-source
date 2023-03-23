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
	
	/// Constructs an empty rest pose.
	rest_pose() noexcept = default;
	
	using pose::update;
	void update(bone_index_type first_index, std::size_t bone_count) override;
	
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

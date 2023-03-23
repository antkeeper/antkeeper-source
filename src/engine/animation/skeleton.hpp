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

#ifndef ANTKEEPER_ANIMATION_SKELETON_HPP
#define ANTKEEPER_ANIMATION_SKELETON_HPP

#include <engine/animation/bone.hpp>
#include <engine/animation/rest-pose.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <unordered_map>
#include <vector>
#include <optional>

/**
 * Skeletal animation skeleton.
 */
class skeleton
{
public:
	/**
	 * Constructs a skeleton.
	 *
	 * @param bone_count Number of bones in the skeleton.
	 */
	explicit skeleton(std::size_t bone_count);
	
	/// Constructs an empty skeleton.
	skeleton();
	
	/**
	 * Updates the rest pose of the skeleton.
	 */
	void update_rest_pose();
	
	/**
	 * Sets the number of bones in the skeleton.
	 *
	 * @param size Number of bones in the skeleton.
	 */
	void set_bone_count(std::size_t size);
	
	/**
	 * Sets the parent of a bone.
	 *
	 * @param child_index Index of the child bone.
	 * @param parent_index Index of the parent bone.
	 *
	 * @warning The index of a child bone must be greater than the index of its parent. Failure to properly index bones will result in incorrect pose concatenation.
	 */
	inline void set_bone_parent(bone_index_type child_index, bone_index_type parent_index)
	{
		m_bone_parents[child_index] = parent_index;
	}
	
	/**
	 * Sets the transform of a bone, relative to its parent bone.
	 *
	 * @param index Index of a bone.
	 * @param transform Bone transform, relative to the parent bone.
	 */
	inline void set_bone_transform(bone_index_type index, const bone_transform_type& transform)
	{
		m_rest_pose.set_relative_transform(index, transform);
	}
	
	/**
	 * Sets the name of a bone.
	 *
	 * @param index Index of a bone.
	 * @param name Name of the bone.
	 */
	inline void set_bone_name(bone_index_type index, hash::fnv1a32_t name)
	{
		m_bone_map[name] = index;
	}
	
	/**
	 * Returns the number of bones in the skeleton.
	 */
	[[nodiscard]] inline std::size_t get_bone_count() const noexcept
	{
		return m_bone_parents.size();
	}
	
	/**
	 * Returns the index of the parent of a bone.
	 *
	 * @param child_index Index of the child bone.
	 *
	 * @return Index of the child bone's parent, or @p index if the child bone has no parent.
	 */
	[[nodiscard]] inline bone_index_type get_bone_parent(bone_index_type child_index) const
	{
		return m_bone_parents[child_index];
	}
	
	/**
	 * Finds the index of a bone from the bone's name.
	 *
	 * @param name Name of a bone.
	 *
	 * @return Index of the bone, or `std::nullopt` if no bone with the given name was found.
	 */
	[[nodiscard]] std::optional<bone_index_type> get_bone_index(hash::fnv1a32_t name) const;
	
	/// Returns the skeleton's rest pose.
	[[nodiscard]] inline const rest_pose& get_rest_pose() const noexcept
	{
		return m_rest_pose;
	}
	
private:
	/// Indices of bone parents.
	std::vector<bone_index_type> m_bone_parents;
	
	/// Rest pose of the skeleton.
	rest_pose m_rest_pose;
	
	/// Map of bone names to bone indices.
	std::unordered_map<hash::fnv1a32_t, bone_index_type> m_bone_map;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_HPP

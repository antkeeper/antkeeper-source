// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_SKELETON_HPP
#define ANTKEEPER_ANIMATION_SKELETON_HPP

#include <engine/animation/bone.hpp>
#include <engine/animation/rest-pose.hpp>
#include <engine/animation/animation-pose.hpp>
#include <unordered_map>
#include <string>
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
	
	/** Constructs an empty skeleton. */
	skeleton();

	/**
	 * Updates the rest pose of the skeleton.
	 */
	void update_rest_pose();
	
	/**
	 * Add one or more bones to the skeleton.
	 *
	 * @param bone_count Number of bones to add.
	 *
	 * @return Index of the first added bone.
	 */
	bone_index_type add_bones(std::size_t bone_count);
	
	/**
	 * Adds a single bone to the skeleton.
	 *
	 * @return Index of the added bone.
	 */
	inline bone_index_type add_bone()
	{
		return add_bones(1);
	}
	
	/**
	 * Adds a single named bone to the skeleton.
	 *
	 * @param name Name of the bone.
	 *
	 * @return Index of the added bone.
	 *
	 * @throw std::invalid_argument Duplicate bone name.
	 */
	bone_index_type add_bone(const std::string& name);
	
	/**
	 * Removes all bones from the skeleton.
	 */
	void remove_bones();
	
	/**
	 * Adds a pose to the skeleton.
	 *
	 * @param name Name of the pose.
	 *
	 * @return Reference to the added pose.
	 *
	 * @throw std::invalid_argument Duplicate pose name.
	 */
	animation_pose& add_pose(const std::string& name);
	
	/**
	 * Removes a pose from the skeleton.
	 *
	 * @param name Name of the pose to remove.
	 *
	 * @throw std::invalid_argument Pose not found.
	 */
	void remove_pose(const std::string& name);
	
	/**
	 * Removes all poses from the skeleton, excluding the rest pose.
	 */
	void remove_poses();

	/**
	 * Sets the name of the skeleton.
	 *
	 * @param name Skeleton name.
	 */
	void set_name(const std::string& name);
	
	/**
	 * Sets the parent of a bone.
	 *
	 * @param child_index Index of the child bone.
	 * @param parent_index Index of the parent bone.
	 *
	 * @warning The index of a child bone must be greater than the index of its parent.
	 *
	 * @throw std::invalid_argument Child bone index precedes parent bone index.
	 */
	void set_bone_parent(bone_index_type child_index, bone_index_type parent_index);
	
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
	 *
	 * @throw std::invalid_argument Duplicate bone name.
	 */
	void set_bone_name(bone_index_type index, const std::string& name);

	/** Returns the name of the skeleton. */
	[[nodiscard]] inline constexpr const std::string& get_name() const noexcept
	{
		return m_name;
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
	[[nodiscard]] std::optional<bone_index_type> get_bone_index(const std::string& name) const;
	
	/// @{
	/**
	 * Finds a pose from the poses's name.
	 *
	 * @param name Name of a pose.
	 *
	 * @return Non-owning pointer to the pose, or `nullptr` if no pose with the given name was found.
	 */
	[[nodiscard]] const animation_pose* get_pose(const std::string& name) const;
	[[nodiscard]] animation_pose* get_pose(const std::string& name);
	/// @}
	
	/** Returns the skeleton's rest pose. */
	[[nodiscard]] inline const rest_pose& get_rest_pose() const noexcept
	{
		return m_rest_pose;
	}
	
private:
	std::string m_name;
	std::vector<bone_index_type> m_bone_parents;
	rest_pose m_rest_pose;
	std::unordered_map<std::string, bone_index_type> m_bone_map;
	std::unordered_map<std::string, animation_pose> m_pose_map;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_HPP

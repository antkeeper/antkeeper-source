// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_SKELETON_HPP
#define ANTKEEPER_ANIMATION_SKELETON_HPP

#include <engine/animation/bone-container.hpp>
#include <engine/animation/skeleton-rest-pose.hpp>
#include <string>

/// Skeletal animation skeleton.
class skeleton
{
public:
	/// Constructs a skeleton.
	/// @param bone_count Number of bones in the skeleton.
	explicit skeleton(std::size_t bone_count);

	/// Constructs an empty skeleton.
	skeleton() = default;

	/// Copy-constructs a skeleton.
	skeleton(const skeleton& other);

	/// Move-constructs a skeleton.
	skeleton(skeleton&& other);

	/// Copy-assigns a skeleton.
	skeleton& operator=(const skeleton& other);

	/// Move-assigns a skeleton.
	skeleton& operator=(skeleton&& other);

	/// Sets the name of the skeleton.
	/// @param name Skeleton name.
	void rename(const std::string& name);
	
	/// Returns the name of the skeleton.
	[[nodiscard]] inline constexpr const auto& name() const noexcept
	{
		return m_name;
	}
	
	/// @name Bones
	/// @{

	/// Returns the bones of the skeleton.
	[[nodiscard]] inline auto& bones() noexcept
	{
		return m_bones;
	}

	/// @copydoc bones()
	[[nodiscard]] inline const auto& bones() const noexcept
	{
		return m_bones;
	}

	/// @}
	/// @name Rest pose
	/// @{

	/// Returns the rest pose of the skeleton.
	[[nodiscard]] inline auto& rest_pose() noexcept
	{
		return m_rest_pose;
	}

	/// @copydoc rest_pose()
	[[nodiscard]] inline const auto& rest_pose() const noexcept
	{
		return m_rest_pose;
	}

	/// @}

private:
	friend class bone_container;
	friend class bone;

	std::string m_name;
	bone_container m_bones;
	skeleton_rest_pose m_rest_pose;
};

#endif // ANTKEEPER_ANIMATION_SKELETON_HPP

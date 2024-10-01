// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_BONE_HPP
#define ANTKEEPER_ANIMATION_BONE_HPP

#include <cstddef>
#include <span>
#include <string>
#include <vector>

class skeleton;

/// Skeleton bone.
class bone
{
public:
	// bone(const bone&) = delete;
	// bone(bone&&) = delete;
	// bone& operator=(const bone&) = delete;
	// bone& operator=(bone&&) = delete;

	/// Changes the name of the bone.
	/// @param name Bone name.
	void rename(const std::string& name);

	/// Returns the name of the bone.
	[[nodiscard]] inline constexpr const std::string& name() const noexcept	
	{
		return m_name;
	}

	/// Returns the length of the bone.
	[[nodiscard]] inline constexpr float& length() noexcept
	{
		return m_length;
	}

	/// @copydoc length()
	[[nodiscard]] inline constexpr const float& length() const noexcept
	{
		return m_length;
	}

	/// @name Hierarchy
	/// @{

	/// Changes the parent of the bone.
	/// @param parent Bone parent, or `nullptr` if the bone should have no parent.
	/// @exception std::invalid_argument Cannot parent a bone to itself.
	/// @exception std::invalid_argument Cannot parent a bone to a bone belonging to another skeleton.
	/// @exception std::invalid_argument Cannot parent a bone to a descendant bone.
	void reparent(bone* parent);

	/// Returns the skeleton to which this bone belongs.
	[[nodiscard]] inline constexpr ::skeleton& skeleton() noexcept
	{
		return *m_skeleton;
	}

	/// @copydoc skeleton()
	[[nodiscard]] inline constexpr const ::skeleton& skeleton() const noexcept
	{
		return *m_skeleton;
	}

	/// Returns the index of the bone within the skeleton.
	/// @note This index may change as bones are added or removed from the skeleton.
	[[nodiscard]] inline constexpr std::size_t index() const noexcept	
	{
		return m_index;
	}

	/// Returns a pointer to the parent bone, or `nullptr` if the bone has no parent.
	[[nodiscard]] inline constexpr bone* parent() noexcept
	{
		return m_parent;
	}

	/// @copydoc parent()
	[[nodiscard]] inline constexpr const bone* parent() const noexcept
	{
		return m_parent;
	}

	/// Returns pointers to the bones which are direct children of this bone.
	[[nodiscard]] inline std::span<bone* const> children() noexcept
	{
		return m_children;
	}

	/// @copydoc children()
	[[nodiscard]] inline std::span<const bone* const> children() const noexcept
	{
		return m_children;
	}

	/// Checks if this bone is an ancestor of another bone.
	/// @param other Bone to check.
	/// @return `true` if this bone is an ancestor of @p other, `false` otherwise.
	[[nodiscard]] bool is_ancestor_of(const bone& other) const noexcept;

	/// @}

private:
	friend class skeleton;
	friend class bone_container;

	::skeleton* m_skeleton{};
	std::size_t m_index{};
	std::string m_name;
	bone* m_parent{};
	std::vector<bone*> m_children;
	float m_length{1.0f};
};

#endif // ANTKEEPER_ANIMATION_BONE_HPP

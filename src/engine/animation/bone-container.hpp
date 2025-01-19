// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_BONE_CONTAINER_HPP
#define ANTKEEPER_ANIMATION_BONE_CONTAINER_HPP

#include <engine/animation/bone.hpp>
#include <memory>
#include <string>
#include <vector>

class skeleton;
class bone;

/// Skeleton bones container.
class bone_container
{
public:
	using value_type = bone;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = std::vector<value_type>::iterator;
	using const_iterator = std::vector<value_type>::const_iterator;
	using reverse_iterator = std::vector<value_type>::reverse_iterator;
	using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;

	/// Constructs a bone container.
	/// @param skeleton Reference to the parent skeleton.
	/// @param bone_count Number of bones in the skeleton.
	bone_container(::skeleton& skeleton, std::size_t bone_count);

	/// Constructs an empty bone container.
	bone_container() = default;

	/// Copy-constructs a bone container.
	bone_container(const bone_container& other);

	/// Move-constructs a bone container.
	bone_container(bone_container&&) = default;

	/// Copy-assigns a bone container.
	bone_container& operator=(const bone_container& other);

	/// Move-assigns a bone container.
	bone_container& operator=(bone_container&&) = default;

	/// @name Lookup
	/// @{

	/// Returns a reference to the bone at the specified index, with bounds checking.
	/// @param i Index of a bone.
	/// @return Reference to the bone at index @p i.
	/// @exception std::out_of_range if @p i >= size().
	[[nodiscard]] inline constexpr reference at(std::size_t i)
	{
		return m_bones.at(i);
	}

	/// @copydoc at(std::size_t)
	[[nodiscard]] inline constexpr const_reference at(std::size_t i) const
	{
		return m_bones.at(i);
	}

	/// Returns a reference to the bone with the specified name.
	/// @param name Name of a bone.
	/// @return Reference to the bone with name @p name.
	/// @exception std::out_of_range Bone not found.
	[[nodiscard]] reference at(const std::string& name);

	/// @copydoc at(const std::string&)
	[[nodiscard]] const_reference at(const std::string& name) const;
	
	/// Returns a reference to the bone at the specified index.
	/// @param i Index of a bone.
	/// @return Reference to the bone at index @p i.
	[[nodiscard]] inline constexpr reference operator[](std::size_t i)
	{
		return m_bones[i];
	}

	/// @copydoc operator[](std::size_t)
	[[nodiscard]] inline constexpr const_reference operator[](std::size_t i) const
	{
		return m_bones[i];
	}

	/// Returns a reference to the bone with the specified name.
	/// @param name Name of a bone.
	/// @return Reference to the bone with name @p name.
	/// @exception std::out_of_range Bone not found.
	[[nodiscard]] inline reference operator[](const std::string& name)
	{
		return at(name);
	}

	/// @copydoc operator[](const std::string&)
	[[nodiscard]] inline const_reference operator[](const std::string& name) const
	{
		return at(name);
	}
	
	/// Returns a reference to the first bone.
	[[nodiscard]] inline constexpr reference front() noexcept
	{
		return m_bones.front();
	}

	/// @copydoc front()
	[[nodiscard]] inline constexpr const_reference front() const noexcept
	{
		return m_bones.front();
	}
	
	/// Returns a reference to the last bone.
	[[nodiscard]] inline constexpr reference back() noexcept
	{
		return m_bones.back();
	}

	/// @copydoc back()
	[[nodiscard]] inline constexpr const_reference back() const noexcept
	{
		return m_bones.back();
	}

	/// Finds a bone by name.
	/// @param name Name of a bone.
	/// @return Iterator to the bone with the given name, or end() if no such bone was found.
	[[nodiscard]] iterator find(const std::string& name);

	/// @copydoc find(const std::string&)
	[[nodiscard]] const_iterator find(const std::string& name) const;

	/// Checks if a bone with the given name is in the container.
	/// @param name Name of a bone.
	/// @return `true` if a bone with the given name was found, `false` otherwise.
	[[nodiscard]] inline bool contains(const std::string& name) const
	{
		return find(name) != m_bones.end();
	}

	/// @}
	/// @name Iterators
	/// @{
	
	/// Returns an iterator to the first bone.
	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return m_bones.begin();
	}

	/// @copydoc begin()
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return m_bones.begin();
	}

	/// @copydoc begin()
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return m_bones.cbegin();
	}
	
	/// Returns an iterator to the bone following the last bone.
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return m_bones.end();
	}

	/// @copydoc end()
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return m_bones.end();
	}

	/// @copydoc end()
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return m_bones.cend();
	}
	
	/// Returns a reverse iterator to the first bone of the reversed container.
	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return m_bones.rbegin();
	}

	/// @copydoc rbegin()
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return m_bones.rbegin();
	}

	/// @copydoc rbegin()
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return m_bones.crbegin();
	}
	
	/// Returns a reverse iterator to the bone following the last bone of the reversed container.
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return m_bones.rend();
	}

	/// @copydoc rend()
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return m_bones.rend();
	}

	/// @copydoc rend()
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return m_bones.crend();
	}
	
	/// @}
	/// @name Capacity
	/// @{
	
	/// Returns `true` if the container is empty, `false` otherwise.
	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return m_bones.empty();
	}
	
	/// Returns the number of bones in the container.
	[[nodiscard]] inline constexpr std::size_t size() const noexcept
	{
		return m_bones.size();
	}

	// Increases the capacity of the container to a value greater or equal to @p new_cap.
	// @param new_cap New minimum capacity of the container, in number of bones.
	// @warning If @p new_cap is greater than capacity(), all iterators and references to bones will be invalidated.
	// void reserve(std::size_t new_cap);

	/// Returns the number of bones for which the container has currently allocated space.
	[[nodiscard]] inline constexpr std::size_t capacity() const noexcept
	{
		return m_bones.capacity();
	}
	
	/// @}
	/// @name Modifiers
	/// @{

	// Erases all bones from the container.
	// void clear();

	// Erases a bone from the bone container.
	// @param it Iterator to the bone to erase.
	// @return Iterator to the bone taking the place of the erased bone.
	// @note If the bone to be erased has any children, the children will be reparented to the parent of the bone to be erased.
	// iterator erase(iterator it);

	// @copydoc erase(iterator)
	// iterator erase(const_iterator it);

	// Appends a new bone to the end of the container.
	// @param name Name of the bone.
	// @return Reference to the new bone.
	// @warning If the new size() is greater than capacity(), all iterators and references to bones will be invalidated.
	// reference emplace_back(const std::string& name);

	/// @}
	
private:
	friend class skeleton;

	skeleton* m_skeleton{};
	std::vector<bone> m_bones;
};

#endif // ANTKEEPER_ANIMATION_BONE_CONTAINER_HPP

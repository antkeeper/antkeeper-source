// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/bone-container.hpp>
#include <engine/animation/skeleton.hpp>
#include <algorithm>
#include <format>
#include <stdexcept>
#include <utility>

bone_container::bone_container(::skeleton& skeleton, std::size_t bone_count):
	m_skeleton{&skeleton},
	m_bones(bone_count)
{
	for (std::size_t i = 0; i < bone_count; ++i)
	{
		m_bones[i].m_skeleton = m_skeleton;
		m_bones[i].m_index = i;
	}
}

bone_container::bone_container(const bone_container& other)
{
	*this = other;
}

bone_container& bone_container::operator=(const bone_container& other)
{
	m_skeleton = other.m_skeleton;
	m_bones = other.m_bones;

	for (auto& bone: m_bones)
	{
		// Fix parent pointer
		if (bone.m_parent)
		{
			bone.m_parent = &m_bones[bone.m_parent->index()];
		}
		
		// Fix child pointers
		for (auto& child: bone.m_children)
		{
			child = &m_bones[child->index()];
		}
	}

	return *this;
}

bone_container::reference bone_container::at(const std::string& name)
{
	for (auto& bone: m_bones)
	{
		if (bone.name() == name)
		{
			return bone;
		}
	}

	throw std::out_of_range(std::format("Bone not found (\"{}\").", name));
}

bone_container::const_reference bone_container::at(const std::string& name) const
{
	for (const auto& bone: m_bones)
	{
		if (bone.name() == name)
		{
			return bone;
		}
	}

	throw std::out_of_range(std::format("Bone not found (\"{}\").", name));
}

bone_container::iterator bone_container::find(const std::string& name)
{
	for (auto it = m_bones.begin(); it != m_bones.end(); ++it)
	{
		if (it->name() == name)
		{
			return it;
		}
	}

	return m_bones.end();
}

bone_container::const_iterator bone_container::find(const std::string& name) const
{
	for (auto it = m_bones.begin(); it != m_bones.end(); ++it)
	{
		if (it->name() == name)
		{
			return it;
		}
	}

	return m_bones.end();
}

// void bone_container::reserve(std::size_t new_cap)
// {
// 	if (new_cap > m_bones.capacity())
// 	{
// 		// Allocate indices of parents
// 		std::vector<std::size_t> parents(m_bones.size());
//
// 		for (auto& child: m_bones)
// 		{
// 			if (child.parent())
// 			{
// 				// Store index of parent
// 				parents[child.index()] = child.parent()->index();
// 			}
//
// 			// Clear children
// 			child.m_children.clear();
// 		}
//
// 		// Reserve new capacity
// 		m_bones.reserve(new_cap);
//
// 		for (auto& child: m_bones)
// 		{
// 			if (child.parent())
// 			{
// 				// Update child's pointer to parent
// 				child.m_parent = &m_bones[parents[child.index()]];
//
// 				// Update parent's pointer to child
// 				child.m_parent->m_children.emplace_back(&child);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// Reserve new capacity
// 		m_bones.reserve(new_cap);
// 	}
// }
//
// void bone_container::clear()
// {
// 	m_bones.clear();
//
// 	if (m_skeleton)
// 	{
// 		// Resize rest pose
// 		m_skeleton->m_rest_pose.resize();
// 	}
// }
//
// bone_container::iterator bone_container::erase(iterator it)
// {
// 	if (it->m_parent)
// 	{
// 		// Remove bone to be erased from its parent's children
// 		auto child_it = std::find(it->m_parent->m_children.begin(), it->m_parent->m_children.end(), &(*it));
// 		if (child_it != it->m_parent->m_children.end())
// 		{
// 			it->m_parent->m_children.erase(child_it);
// 		}
// 	}
//
// 	// Reparent children of bone to be erased
// 	for (auto child: it->m_children)
// 	{
// 		child->m_parent = it->m_parent;
//
// 		if (it->m_parent)
// 		{
// 			it->m_parent->m_children.emplace_back(child);
// 		}
// 	}
//
// 	// Store index of bone to be erased
// 	const auto erase_index = it->index();
//
// 	// If bone to be erased is not at the back of the container
// 	if (it != std::prev(m_bones.end()))
// 	{
// 		// Swap last bone in container with bone to be erased
// 		*it = std::move(m_bones.back());
//
// 		if (m_skeleton)
// 		{
// 			// Swap rest pose transforms
// 			m_skeleton->m_rest_pose.m_relative_transforms[it->m_index] = std::move(m_skeleton->m_rest_pose.m_relative_transforms[erase_index]);
// 			m_skeleton->m_rest_pose.m_absolute_transforms[it->m_index] = std::move(m_skeleton->m_rest_pose.m_absolute_transforms[erase_index]);
// 			m_skeleton->m_rest_pose.m_inverse_absolute_transforms[it->m_index] = std::move(m_skeleton->m_rest_pose.m_inverse_absolute_transforms[erase_index]);
// 		}
//
// 		// Update index of swapped bone
// 		it->m_index = erase_index;
//
// 		if (it->m_parent)
// 		{
// 			// Update pointer in swapped bone's parent's children
// 			for (auto& child: it->m_parent->m_children)
// 			{
// 				if (child == &m_bones.back())
// 				{
// 					child = &(*it);
// 					break;
// 				}
// 			}
// 		}
//
// 		// Update pointer in swapped bone's children
// 		for (auto& child: it->m_children)
// 		{
// 			child->m_parent = &(*it);
// 		}
// 	}
//
// 	// Erase bone from back of the container
// 	m_bones.pop_back();
//
// 	if (m_skeleton)
// 	{
// 		// Resize rest pose
// 		m_skeleton->m_rest_pose.resize();
// 	}
//
// 	return (erase_index == m_bones.size() ? m_bones.end() : m_bones.begin() + erase_index);
// }
//
// bone_container::iterator bone_container::erase(const_iterator it)
// {
// 	return erase(begin() + std::distance(cbegin(), it));
// }
//
// bone_container::reference bone_container::emplace_back(const std::string& name)
// {
// 	bone* result{};
//
// 	// if new size() is greater than capacity()
// 	if (m_bones.size() == m_bones.capacity())
// 	{
// 		// Allocate indices of parents
// 		std::vector<std::size_t> parents(m_bones.size());
//
// 		for (auto& child: m_bones)
// 		{
// 			if (child.parent())
// 			{
// 				// Store index of parent
// 				parents[child.index()] = child.parent()->index();
// 			}
//
// 			// Clear children
// 			child.m_children.clear();
// 		}
//
// 		// Allocate new bone (resulting in reallocation of bones)
// 		result = &m_bones.emplace_back();
//
// 		for (auto& child: m_bones)
// 		{
// 			if (child.parent())
// 			{
// 				// Update child's pointer to parent
// 				child.m_parent = &m_bones[parents[child.index()]];
//
// 				// Update parent's pointer to child
// 				child.m_parent->m_children.emplace_back(&child);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		// Allocate new bone (no reallocation)
// 		result = &m_bones.emplace_back();
// 	}
//
// 	// Init new bone
// 	result->m_skeleton = m_skeleton;
// 	result->m_index = m_bones.size() - 1;
// 	result->m_name = name;
//
// 	if (m_skeleton)
// 	{
// 		// Resize rest pose
// 		m_skeleton->m_rest_pose.resize();
// 	}
//
// 	return *result;
// }

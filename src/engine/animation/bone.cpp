// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/bone.hpp>
#include <stdexcept>

void bone::rename(const std::string& name)
{
	m_name = name;
}

void bone::reparent(bone* parent)
{
	// Ignore if parenting to current parent
	if (parent == m_parent)
	{
		return;
	}

	// Check if parenting to self
	if (parent == this)
	{
		throw std::invalid_argument("Cannot parent a bone to itself.");
	}

	if (parent)
	{
		// Check if parenting to bone of another skeleton
		if (parent->m_skeleton != m_skeleton)
		{
			throw std::invalid_argument("Cannot parent a bone to a bone belonging to another skeleton.");
		}

		// Check if parenting to a descendant bone
		if (is_ancestor_of(*parent))
		{
			throw std::invalid_argument("Cannot parent a bone to a descendant bone.");
		}
	}

	if (m_parent)
	{
		// Remove this bone from its parent's children
		for (auto it = m_parent->m_children.begin(); it != m_parent->m_children.end(); ++it)
		{
			if (*it == this)
			{
				m_parent->m_children.erase(it);
				break;
			}
		}
	}

	if (parent)
	{
		// Add this bone to its new parent's children
		parent->m_children.emplace_back(this);
	}

	// Assign new parent
	m_parent = parent;
}

bool bone::is_ancestor_of(const bone& other) const noexcept
{
	for (auto current = other.parent(); current != nullptr; current = current->parent())
	{
		if (current == this)
		{
			return true;
		}
	}

	return false;
}


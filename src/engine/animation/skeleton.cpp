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

#include <engine/animation/skeleton.hpp>
#include <stdexcept>

skeleton::skeleton(std::size_t bone_count):
	m_bone_parents(bone_count, 0),
	m_rest_pose(*this)
{}

skeleton::skeleton():
	skeleton(0)
{}

void skeleton::update_rest_pose()
{
	m_rest_pose.update();
}

bone_index_type skeleton::add_bones(std::size_t bone_count)
{
	const bone_index_type first_bone_index = static_cast<bone_index_type>(m_bone_parents.size());
	
	m_bone_parents.resize(m_bone_parents.size() + bone_count, 0);
	m_rest_pose.resize();
	
	return first_bone_index;
}

bone_index_type skeleton::add_bone(hash::fnv1a32_t name)
{
	const bone_index_type bone_index = add_bone();
	
	set_bone_name(bone_index, name);
	
	return bone_index;
}

void skeleton::remove_bones()
{
	m_bone_parents.clear();
	m_bone_map.clear();
	m_rest_pose.resize();
}

animation_pose& skeleton::add_pose(hash::fnv1a32_t name)
{
	const auto [iterator, inserted] = m_pose_map.emplace(name, *this);
	
	if (!inserted)
	{
		throw std::invalid_argument("Duplicate pose name");
	}
	
	return iterator->second;
}

void skeleton::remove_pose(hash::fnv1a32_t name)
{
	if (!m_pose_map.erase(name))
	{
		throw std::invalid_argument("Pose not found");
	}	
}

void skeleton::remove_poses()
{
	m_pose_map.clear();
}

void skeleton::set_bone_parent(bone_index_type child_index, bone_index_type parent_index)
{
	if (child_index < parent_index)
	{
		throw std::invalid_argument("Child bone index precedes parent bone index");
	}
	
	m_bone_parents[child_index] = parent_index;
}

void skeleton::set_bone_name(bone_index_type index, hash::fnv1a32_t name)
{
	if (auto i = m_bone_map.find(name); i != m_bone_map.end())
	{
		if (i->second != index)
		{
			throw std::invalid_argument("Duplicate bone name");
		}
	}
	else
	{
		m_bone_map[name] = index;
	}
}

std::optional<bone_index_type> skeleton::get_bone_index(hash::fnv1a32_t name) const
{
	if (auto i = m_bone_map.find(name); i != m_bone_map.end())
	{
		return i->second;
	}
	
	return std::nullopt;
}

const animation_pose* skeleton::get_pose(hash::fnv1a32_t name) const
{
	if (auto i = m_pose_map.find(name); i != m_pose_map.end())
	{
		return &i->second;
	}
	
	return nullptr;
}

animation_pose* skeleton::get_pose(hash::fnv1a32_t name)
{
	if (auto i = m_pose_map.find(name); i != m_pose_map.end())
	{
		return &i->second;
	}
	
	return nullptr;
}

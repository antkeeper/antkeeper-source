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

skeleton::skeleton(std::size_t bone_count):
	m_bone_parents(bone_count, 0),
	m_bind_pose(*this)
{}

skeleton::skeleton():
	skeleton(0)
{}

void skeleton::update_bind_pose()
{
	m_bind_pose.update();
}

void skeleton::set_bone_count(std::size_t bone_count)
{
	m_bone_parents.resize(bone_count, 0);
	m_bind_pose.set_skeleton(this);
}

std::optional<typename skeleton::bone_index_type> skeleton::get_bone_index(hash::fnv1a32_t name) const
{
	if (auto i = m_bone_map.find(name); i != m_bone_map.end())
	{
		return i->second;
	}
	
	return std::nullopt;
}

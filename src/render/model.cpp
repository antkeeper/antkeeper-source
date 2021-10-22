/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "render/model.hpp"

namespace render {

model::model():
	bounds({0, 0, 0}, {0, 0, 0})
{}

model::~model()
{
	for (model_group* group: groups)
	{
		delete group;
	}
}

model_group* model::add_group(const std::string& name)
{
	if (!name.empty())
	{
		if (auto it = group_map.find(name); it != group_map.end())
		{
			return it->second;
		}
	}

	model_group* group = new model_group();
	group->index = groups.size();
	group->name = name;
	group->material = nullptr;
	group->drawing_mode = gl::drawing_mode::triangles;
	group->start_index = 0;
	group->index_count = 0;

	groups.push_back(group);

	if (!name.empty())
	{
		group_map[name] = group;
	}

	return group;
}

bool model::remove_group(const std::string& name)
{
	if (auto it = group_map.find(name); it != group_map.end())
	{
		return remove_group(it->second);
	}

	return false;
}

bool model::remove_group(model_group* group)
{
	// Remove from group map
	if (!group->name.empty())
	{
		if (auto it = group_map.find(group->name); it != group_map.end())
		{
			group_map.erase(it);
		}
	}

	// Adjust indices of groups after this group
	for (std::size_t i = group->index + 1; i < groups.size(); ++i)
	{
		--groups[i]->index;
	}

	// Remove from groups
	groups.erase(groups.begin() + group->index);

	// Deallocate group
	delete group;

	return true;
}

const model_group* model::get_group(const std::string& name) const
{
	if (auto it = group_map.find(name); it != group_map.end())
	{
		return it->second;
	}

	return nullptr;
}

model_group* model::get_group(const std::string& name)
{
	if (auto it = group_map.find(name); it != group_map.end())
	{
		return it->second;
	}

	return nullptr;
}

} // namespace render

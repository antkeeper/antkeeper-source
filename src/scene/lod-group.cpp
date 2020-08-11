/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "scene/lod-group.hpp"
#include "scene/camera.hpp"

lod_group::lod_group(std::size_t level_count):
	bounds(get_translation(), get_translation())
{
	resize(level_count);
}

lod_group::lod_group():
	lod_group(1)
{}

void lod_group::resize(std::size_t level_count)
{
	levels.resize(level_count);
}

std::size_t lod_group::select_lod(const ::camera& camera) const
{
	float distance = camera.get_view_frustum().get_near().signed_distance(get_translation());
	
	if (distance < 300.0f)
		return 0;
	else if (distance < 500.0f)
		return 1;
	else if (distance < 600.0f)
		return 2;
	
	return 3;
}

void lod_group::add_object(std::size_t level, scene_object_base* object)
{
	levels[level].push_back(object);
}

void lod_group::remove_object(std::size_t level, scene_object_base* object)
{
	levels[level].remove(object);
}

void lod_group::remove_objects(std::size_t level)
{
	levels[level].clear();
}

void lod_group::update_bounds()
{
	bounds = {get_translation(), get_translation()};
}

void lod_group::transformed()
{
	update_bounds();
}

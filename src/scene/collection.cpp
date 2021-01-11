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

#include "scene/collection.hpp"
#include "scene/object.hpp"

namespace scene {

void collection::add_object(object_base* object)
{
	objects.push_back(object);
	object_map[object->get_object_type_id()].push_back(object);
}

void collection::remove_object(object_base* object)
{
	objects.remove(object);
	object_map[object->get_object_type_id()].remove(object);
}

void collection::remove_objects()
{
	objects.clear();
	object_map.clear();
}

void collection::update_tweens()
{
	for (object_base* object: objects)
	{
		object->update_tweens();
	}
}

} // namespace scene

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

#include "scene/scene.hpp"
#include "scene/scene-object.hpp"
#include "scene/light.hpp"
#include "scene/camera.hpp"
#include "scene/model-instance.hpp"

void scene::add_object(scene_object_base* object)
{
	objects.push_back(object);
	object_map[object->get_object_type_id()].push_back(object);
}

void scene::remove_object(scene_object_base* object)
{
	objects.remove(object);
	object_map[object->get_object_type_id()].remove(object);
}

void scene::remove_objects()
{
	objects.clear();
	object_map.clear();
}

void scene::update_tweens()
{
	for (scene_object_base* object: objects)
	{
		object->update_tweens();
	}
}

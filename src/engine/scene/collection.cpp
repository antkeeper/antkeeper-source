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

#include <engine/scene/collection.hpp>

namespace scene {

void collection::add_object(object_base& object)
{
	m_objects.emplace_back(&object);
	m_object_map[object.get_object_type_id()].emplace_back(&object);
}

void collection::remove_object(const object_base& object)
{
	std::erase(m_objects, &object);
	std::erase(m_object_map[object.get_object_type_id()], &object);
}

void collection::remove_objects()
{
	m_objects.clear();
	m_object_map.clear();
}

} // namespace scene

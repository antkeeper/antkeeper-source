// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

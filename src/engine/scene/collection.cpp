// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.collection;
import engine.debug.log;

namespace engine::scene
{
	void collection::add_object(object_base& object)
	{
		if (m_object_set.contains(&object))
		{
			log_warning("Ignoring request to add existing object ({}) to scene collection.", static_cast<const void*>(&object));
		}
		else
		{
			m_objects.emplace_back(&object);
			m_object_set.emplace(&object);
			m_object_map[object.get_object_type_id()].emplace_back(&object);
		}
	}

	void collection::remove_object(const object_base& object)
	{
		if (!m_object_set.contains(&object))
		{
			log_warning("Ignoring request to remove non-existing object ({}) from scene collection.", static_cast<const void*>(&object));
		}
		else
		{
			std::erase(m_objects, &object);
			m_object_set.erase(&object);
			std::erase(m_object_map[object.get_object_type_id()], &object);
		}
	}

	void collection::remove_objects()
	{
		m_objects.clear();
		m_object_map.clear();
	}
}

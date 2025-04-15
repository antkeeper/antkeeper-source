// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.entity.clone;
import engine.entity.id;
import engine.entity.registry;

namespace engine::entity
{
	void clone(entity::registry& registry, entity::id source, entity::id destination)
	{
		for (auto&& it: registry.storage())
		{
			if (auto& storage = it.second; storage.contains(source))
			{
				storage.push(destination, storage.value(source));
			}
		}
	}
}

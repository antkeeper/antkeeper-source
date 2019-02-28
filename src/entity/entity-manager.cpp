/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "entity-manager.hpp"
#include "component-manager.hpp"

EntityManager::EntityManager(ComponentManager* componentManager):
	componentManager(componentManager)
{}

EntityManager::~EntityManager()
{}


EntityID EntityManager::createEntity()
{
	return idPool.reserveNextID();
}

bool EntityManager::createEntity(EntityID id)
{
	if (idPool.isReserved(id))
	{
		return false;
	}

	idPool.reserveID(id);
	
	return true;
}

bool EntityManager::destroyEntity(EntityID id)
{
	if (!idPool.isReserved(id))
	{
		return false;
	}

	// Delete components
	ComponentMap* components = componentManager->getComponents(id);
	for (auto it = components->begin(); it != components->end(); it = components->begin())
	{
		ComponentBase* component = it->second;
		componentManager->removeComponent(id, component->getComponentType());
		delete component;
	}

	// Free ID
	idPool.freeID(id);

	return true;
}


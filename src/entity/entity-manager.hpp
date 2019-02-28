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

#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include "entity-id.hpp"
#include "entity-id-pool.hpp"

class ComponentManager;

/**
 * Manages the creation and destruction of entities.
 */
class EntityManager
{
public:
	/**
	 * Creates an entity manager.
	 *
	 * @param componentManager Component manager with which to associate this entity manag.er
	 */
	EntityManager(ComponentManager* componentManager);
	
	/**
	 * Destroys an entity manager.
	 */
	~EntityManager();
	
	/**
	 * Creates an entity with the next available ID.
	 *
	 * @return ID of the created entity.
	 */
	EntityID createEntity();

	/**
	 * Creates an entity with the specified ID.
	 *
	 * @param id ID of the entity to be created.
	 * @return `true` if the entity was created, and `false` if an entity with the specified ID already exists.
	 */
	bool createEntity(EntityID id);
	
	/**
	 * Destroys an entity with the specified ID.
	 *
	 * @param id ID of the entity to be destroyed.
	 *
	 * @return `true` if the entity was destroyed, and `false` if an invalid ID was supplied.
	 */
	bool destroyEntity(EntityID id);
	
	/**
	 * Returns the component manager associated with this entity manager.
	 */
	const ComponentManager* getComponentManager() const;

	/// @copydoc EntityManager::getComponentManager() const
	ComponentManager* getComponentManager();
	
private:
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	EntityIDPool idPool;
	ComponentManager* componentManager;
};

inline const ComponentManager* EntityManager::getComponentManager() const
{
	return componentManager;
}

inline ComponentManager* EntityManager::getComponentManager()
{
	return componentManager;
}

#endif // ENTITY_MANAGER_HPP


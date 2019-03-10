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

#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include "entity-id.hpp"
#include "component.hpp"
#include <list>
#include <map>

class ComponentObserver;

/// Maps component types to components.
typedef std::map<ComponentType, ComponentBase*> ComponentMap;

/// Maps entity IDs to a component map.
typedef std::map<EntityID, ComponentMap> EntityComponentMap;

/**
 * Manages the aggregation of components which make up entities.
 */
class ComponentManager
{
public:
	/**
	 * Creates an instance of ComponentManager.
	 */
	ComponentManager() = default;
	
	/**
	 * Destroys an instance of ComponentManager.
	 */
	~ComponentManager() = default;
	
	/**
	 * Adds a ComponentObserver.
	 *
	 * @param observer Pointer to the observer to add.
	 */
	void addComponentObserver(ComponentObserver* observer);
	
	/**
	 * Removes a ComponentObserver.
	 *
	 * @param observer Pointer to the observer to remove.
	 */
	void removeComponentObserver(ComponentObserver* observer);
	
	/**
	 * Adds a component to the specified entity.
	 *
	 * @param entity Specifies an entity.
	 * @param component Specifies the component to add.
	 */
	void addComponent(EntityID entity, ComponentBase* component);
	
	/**
	 * Removes a component from the specified entity.
	 *
	 * @param entity Specifies an entity.
	 * @param type Specifies the type of component.
	 *
	 * @return Pointer to the removed component.
	 */
	ComponentBase* removeComponent(EntityID entity, ComponentType type);
	
	/**
	 * Returns the specified component of an entity.
	 *
	 * @param entity Specifies an entity.
	 * @param type Specifies the type of component.
	 *
	 * @return Pointer to the component, or `nullptr` if the specified component was not found.
	 */
	ComponentBase* getComponent(EntityID entity, ComponentType type);

	/**
	 * Returns the specified component of an entity.
	 *
	 * @param entity Specifies an entity.
	 * @tparam type Specifies the component type.
	 *
	 * @return Pointer to the component, or `nullptr` if the specified component was not found.
	 */
	template <typename T>
	T* getComponent(EntityID entity);
	
	/**
	 * Returns the component map of the specified entity.
	 *
	 * @param entity Specifies an entity.
	 *
	 * @return Pointer to the component map.
	 */
	ComponentMap* getComponents(EntityID entity);
	
private:
	ComponentManager(const ComponentManager&) = delete;
	ComponentManager& operator=(const ComponentManager&) = delete;
	
	EntityComponentMap entityMap;
	std::list<ComponentObserver*> componentObservers;
};

template <typename T>
T* ComponentManager::getComponent(EntityID entity)
{
	ComponentMap& componentMap = entityMap[entity];
	
	auto it = componentMap.find(T::TYPE);
	if (it == componentMap.end())
	{
		return nullptr;
	}
	
	return static_cast<T*>(it->second);
}

inline ComponentMap* ComponentManager::getComponents(EntityID entity)
{
	return &entityMap[entity];
}

#endif // COMPONENT_MANAGER_HPP

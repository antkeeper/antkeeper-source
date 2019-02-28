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

#ifndef COMPONENT_OBSERVER_HPP
#define COMPONENT_OBSERVER_HPP

#include "entity-id.hpp"

class ComponentBase;
class ComponentManager;

/**
 * Abstract base class for component observers.
 */
class ComponentObserver
{
public:
	/**
	 * Creates a component observer.
	 *
	 * @param componentManager Specifies the component manager with which to associate this component observer.
	 */
	ComponentObserver(ComponentManager* componentManager);

	/**
	 * Destroys a component observer.
	 */
	virtual ~ComponentObserver();
	
protected:
	ComponentManager* componentManager;
	
private:
	friend class ComponentManager;
	
	/**
	 * Called after a component is added to an entity.
	 *
	 * @param entity Specifies the entity with which the component is associated.
	 * @param component Specifies the component added.
	 */
	virtual void componentAdded(EntityID entity, ComponentBase* component) = 0;
	
	/**
	 * Called after a component is removed from an entity.
	 *
	 * @param entity Specifies the entity with which the component is associated.
	 * @param component Specifies the component removed.
	 */
	virtual void componentRemoved(EntityID entity, ComponentBase* component) = 0;
};

#endif // COMPONENT_OBSERVER_HPP

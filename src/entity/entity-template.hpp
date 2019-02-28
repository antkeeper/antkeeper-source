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

#ifndef ENTITY_TEMPLATE_HPP
#define ENTITY_TEMPLATE_HPP

#include <list>
#include "entity-id.hpp"

class ComponentBase;
class ComponentManager;

/**
 * A template which can be applied to entities.
 */
class EntityTemplate
{
public:
	/**
	 * Creates an entity template.
	 *
	 * @param components List of components which make up the template. The components in the list will be cloned and the cloned data managed by this template.
	 */
	EntityTemplate(const std::list<ComponentBase*>& components);

	/**
	 * Destroys an entity template.
	 */
	~EntityTemplate();

	/**
	 * Applies the template to an entity.
	 *
	 * @param entity ID of an entity to which the template should be applied.
	 * @param componentManager Component manager with which the entity is associated.
	 */
	void apply(EntityID entity, ComponentManager* componentManager);

private:
	std::list<ComponentBase*> components;
};

#endif // ENTITY_TEMPLATE_HPP


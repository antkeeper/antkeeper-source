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

#include "component.hpp"
#include "component-manager.hpp"
#include "entity-template.hpp"

EntityTemplate::EntityTemplate(const std::list<ComponentBase*>& components)
{
	for (ComponentBase* component: components)
	{
		this->components.push_back(component->clone());
	}
}

EntityTemplate::~EntityTemplate()
{
	for (ComponentBase* component: components)
	{
		delete component;
	}
}

void EntityTemplate::apply(EntityID entity, ComponentManager* componentManager)
{
	for (ComponentBase* component: components)
	{
		ComponentBase* oldComponent = componentManager->getComponent(entity, component->getComponentType());
		if (oldComponent != nullptr)
		{
			componentManager->removeComponent(entity, component->getComponentType());
			delete oldComponent;
		}

		componentManager->addComponent(entity, component->clone());
	}
}


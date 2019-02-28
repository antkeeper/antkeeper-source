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

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

enum class ComponentType;

/**
 * Abstract base class for entity components.
 */
class ComponentBase
{
public:
	/**
	 * Destroys a component.
	 */
	virtual ~ComponentBase() = default;

	/**
	 * Clones the component.
	 */
	virtual ComponentBase* clone() const = 0;
	
	/**
	 * Returns the component type.
	 */
	virtual ComponentType getComponentType() const = 0;
};

/**
 * Abstract templated class for entity components.
 */
template <ComponentType type>
class Component: public ComponentBase
{
public:
	static const ComponentType TYPE;

	/**
	 * Destroys a component.
	 */
	virtual ~Component() = default;
	
	/**
	 * Returns the component type.
	 */
	virtual ComponentType getComponentType() const final;
};

template <ComponentType type>
const ComponentType Component<type>::TYPE = type;

template <ComponentType type>
inline ComponentType Component<type>::getComponentType() const
{
	return type;
}

#endif // COMPONENT_HPP


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

#ifndef SYSTEM_HPP
#define SYSTEM_HPP

class ComponentManager;

/**
 * Abstract base class for entity systems.
 */
class System
{
public:
	/**
	 * Creates a system.
	 *
	 * @param componentManager Component manager with which to associate the system.
	 */
	System(ComponentManager* componentManager);

	/**
	 * Destroys a system.
	 */
	virtual ~System() = default;

	/**
	 * Updates the system.
	 *
	 * @param t Total elapsed time, in seconds.
	 * @param dt Time elapsed since last update, in seconds.
	 */
	virtual void update(float t, float dt) = 0;

protected:
	ComponentManager* componentManager;
};

#endif // SYSTEM_HPP


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

#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include <list>

class System;

/**
 * Manages a series of systems.
 */
class SystemManager
{
public:
	/**
	 * Creates a system manager.
	 */
	SystemManager();

	/**
	 * Destroys a system manager.
	 */
	~SystemManager();

	/**
	 * Updates all systems. Systems will be updated in the order that they were added to the system manager.
	 *
	 * @param t Total elapsed time, in seconds.
	 * @param dt Time elapsed since last update, in seconds.
	 */
	void update(float t, float dt);

	/**
	 * Adds a system to the system manager.
	 *
	 * @param system System to add. 
	 */
	void addSystem(System* system);

	/**
	 * Removes a system from the system manager.
	 */
	void removeSystem(System* system);

private:
	std::list<System*> systems;
};

#endif // SYSTEM_MANAGER_HPP


/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_ECS_UPDATABLE_SYSTEM_HPP
#define ANTKEEPER_ECS_UPDATABLE_SYSTEM_HPP

namespace ecs {

/**
 * Abstract base class for updatable systems.
 */
class updatable_system
{
public:
	/**
	 * Perform's a system's update() function.
	 *
	 * @param t Total elapsed time, in seconds.
	 * @param dt Delta time, in seconds.
	 * @param registry Entity registry.
	 */
	virtual void update(double t, double dt) = 0;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_UPDATABLE_SYSTEM_HPP


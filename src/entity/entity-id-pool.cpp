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

#include "entity-id-pool.hpp"

EntityIDPool::EntityIDPool():
	nextID(0)
{}

EntityIDPool::~EntityIDPool()
{}

EntityID EntityIDPool::reserveNextID()
{
	EntityID id;

	if (!availableIDs.empty())
	{
		id = *availableIDs.begin();
		availableIDs.erase(availableIDs.begin());
		reservedIDs.insert(id);
	}
	else
	{
		id = nextID;
		reservedIDs.insert(id);
		findNextID();
	}

	return id;
}

void EntityIDPool::reserveID(EntityID id)
{
	availableIDs.erase(id);
	reservedIDs.insert(id);
	
	if (nextID == id)
	{
		findNextID();
	}
}

inline void EntityIDPool::findNextID()
{
	do
	{
		++nextID;
	}
	while (reservedIDs.find(nextID) != reservedIDs.end());
}


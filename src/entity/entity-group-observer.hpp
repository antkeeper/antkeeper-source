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

#ifndef ENTITY_GROUP_OBSERVER_HPP
#define ENTITY_GROUP_OBSERVER_HPP

#include "entity-group-member.hpp"

/**
 * Abstract base class for entity group observers, which are notified each time a member is registered or unregistered from the group.
 *
 * @tparam T Set of components which are required for group membership.
 */
template <typename... T>
class EntityGroupObserver
{
public:
	typedef EntityGroupMember<T...> Member;

	/**
	 * Called each time an entity joins the entity group by obtaining the necessary component types.
	 *
	 * @param entity Entity ID of the new member.
	 */
	virtual void memberRegistered(const Member* member) = 0;

	/**
	 * Called each time an entity leaves an the entity group by no longer possessing the necessary component types.
	 *
	 * @param entity Entity ID of the former member.
	 */
	virtual void memberUnregistered(const Member* member) = 0;
};

#endif // ENTITY_GROUP_OBSERVER_HPP	


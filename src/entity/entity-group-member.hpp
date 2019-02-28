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

#ifndef ENTITY_GROUP_MEMBER_HPP
#define ENTITY_GROUP_MEMBER_HPP

#include "entity-id.hpp"
#include <tuple>
#include <type_traits>

/**
 * A group of entities which share a set of specified component types.
 *
 * @tparam T Set of components which are required for group membership.
 */
template <typename... T>
struct EntityGroupMember
{
	/// Entity ID of the group member.
	EntityID entity;
	
	/// A tuple containing pointers to the member's group-related components, in the order specified by the order of the template parameters.
	std::tuple<T*...> components;
};

#endif // ENTITY_GROUP_MEMBER_HPP	


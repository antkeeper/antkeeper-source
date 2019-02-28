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

#ifndef ENTITY_GROUP_HPP
#define ENTITY_GROUP_HPP

#include "component-observer.hpp"
#include "component-manager.hpp"
#include "entity-group-member.hpp"
#include "entity-group-observer.hpp"
#include <list>
#include <map>
#include <set>
#include <tuple>
#include <utility>

enum class ComponentType;

/// A set of component types used to filter entities
typedef std::set<ComponentType> ComponentFilter;

/**
 * Abstract base class for entity groups.
 */
class EntityGroupBase: protected ComponentObserver
{
public:
	/**
	 * Creates a entity group base.
	 *
	 * @param componentManager The component manager with which to associate this entity group.
	 * @param componentFilter Set of component types which an entity must possess in order to join this entity group.
	 */
	EntityGroupBase(ComponentManager* componentManager, const ComponentFilter& componentFilter);

	/// Returns the set of components which an entity must possess in order to join this entity group.
	const ComponentFilter& getComponentFilter() const;

	/**
	 * Returns true if the specified entity is registered with this entity group.
	 *
	 * @param entity ID of the entity to check.
	 */
	virtual bool isRegistered(EntityID entity) const = 0;

private:
	virtual void componentAdded(EntityID entity, ComponentBase* component);
	virtual void componentRemoved(EntityID entity, ComponentBase* component);

	/**
	 * Called each time an entity joins the entity group by obtaining the necessary component types.
	 *
	 * @param entity Entity ID of the new member.
	 */
	virtual void registerMember(EntityID entity) = 0;

	/**
	 * Called each time an entity leaves an the entity group by no longer possessing the necessary component types.
	 *
	 * @param entity Entity ID of the former member.
	 */
	virtual void unregisterMember(EntityID entity) = 0;

	ComponentFilter componentFilter;
};

inline const ComponentFilter& EntityGroupBase::getComponentFilter() const
{
	return componentFilter;
}

/**
 * A group of entities which share a set of specified component types.
 *
 * @tparam T Set of components which are required for group membership.
 */
template <typename... T>
class EntityGroup: public EntityGroupBase
{
public:
	typedef EntityGroupMember<T...> Member;
	typedef EntityGroupObserver<T...> Observer;

	/**
	 * Creates a entity group.
	 *
	 * @param componentManager Component manager with which to associate this entity group.
	 */
	EntityGroup(ComponentManager* componentManager);

	/// Destroys a entity group.
	~EntityGroup();
	
	/**
	 * Adds a group observer.
	 *
	 * @param observer Observer to add.
	 */
	void addGroupObserver(Observer* observer);

	/**
	 * Removes a group observer.
	 *
	 * @param observer Observer to remove.
	 */
	void removeGroupObserver(Observer* observer);

	/// Removes all group observers.
	void removeGroupObservers();

	/// @copydoc EntityGroupBase::isRegistered(EntityID) const
	virtual bool isRegistered(EntityID entity) const;

	/**
	 * Returns the member list.
	 *
	 * @return List of members.
	 */
	const std::list<Member*>* getMembers() const;

	/**
	 * Returns the member with the specified ID.
	 *
	 * @param entity Entity ID of a group member.
	 * @return Member with the specified ID, or nullptr if an entity with that ID is not registered.
	 */
	const Member* getMemberByEntity(EntityID entity) const;

private:
	template <std::size_t index, typename U, typename... V>
	typename std::enable_if<(sizeof...(V) == 0), void>::type attachComponents(Member* member)
	{
		std::get<index>(member->components) = static_cast<U*>(componentManager->getComponent(member->entity, U::TYPE));
	}

	template <std::size_t index, typename U, typename... V>
	typename std::enable_if<(sizeof...(V) > 0), void>::type attachComponents(Member* member)
	{
		std::get<index>(member->components) = static_cast<U*>(componentManager->getComponent(member->entity, U::TYPE));
		attachComponents<index + 1, V...>(member);
	}

	virtual void registerMember(EntityID entity);
	virtual void unregisterMember(EntityID entity);

	std::list<Member*> members;
	std::map<EntityID, Member*> memberMap;
	std::list<Observer*> observers;
};

template <typename... T>
EntityGroup<T...>::EntityGroup(ComponentManager* componentManager):
	EntityGroupBase(componentManager, ComponentFilter({(T::TYPE)...}))
{}

template <typename... T>
EntityGroup<T...>::~EntityGroup()
{
	while (!members.empty())
	{
		Member* member = members.back();

		members.pop_back();
		memberMap.erase(memberMap.find(member->entity));

		for (Observer* observer: observers)
		{
			observer->memberUnregistered(member);
		}

		delete member;
	}
}


template <typename... T>
void EntityGroup<T...>::addGroupObserver(Observer* observer)
{
	observers.push_back(observer);
}

template <typename... T>
void EntityGroup<T...>::removeGroupObserver(Observer* observer)
{
	observers.remove(observer);
}

template <typename... T>
void EntityGroup<T...>::removeGroupObservers()
{
	observers.clear();
}

template <typename... T>
inline bool EntityGroup<T...>::isRegistered(EntityID entity) const
{
	return (memberMap.find(entity) != memberMap.end());
}

template <typename... T>
inline const std::list<typename EntityGroup<T...>::Member*>* EntityGroup<T...>::getMembers() const
{
	return &members;
}

template <typename... T>
inline const typename EntityGroup<T...>::Member* EntityGroup<T...>::getMemberByEntity(EntityID entity) const
{
	auto it = memberMap.find(entity);
	if (it != memberMap.end())
	{
		return it->second;
	}

	return nullptr;
}

template <typename... T>
void EntityGroup<T...>::registerMember(EntityID entity)
{
	Member* member = new Member();
	member->entity = entity;
	attachComponents<0, T...>(member);

	members.push_back(member);
	memberMap[entity] = member;

	for (Observer* observer: observers)
	{
		observer->memberRegistered(member);
	}
}

template <typename... T>
void EntityGroup<T...>::unregisterMember(EntityID entity)
{
	auto it = memberMap.find(entity);
	Member* member = it->second;

	memberMap.erase(it);
	members.remove(member);

	for (Observer* observer: observers)
	{
		observer->memberUnregistered(member);
	}

	delete member;
}

#endif // ENTITY_GROUP_HPP	


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

#ifndef ANTKEEPER_SCENE_COLLECTION_HPP
#define ANTKEEPER_SCENE_COLLECTION_HPP

#include <list>
#include <unordered_map>

namespace scene {

class object_base;

/**
 * Collection of scene objects.
 */
class collection
{
public:
	/**
	 * Adds an object to the collection.
	 *
	 * @param object Object to add.
	 */
	void add_object(object_base* object);
	
	/**
	 * Removes an object from the collection.
	 *
	 * @param object Object to remove.
	 */
	void remove_object(object_base* object);
	
	/// Removes all objects from the collection.
	void remove_objects();
	
	/// Updates the tweens of all objects in the collection.
	void update_tweens();

	/// Returns a list of all objects in the collection.
	const std::list<object_base*>* get_objects() const;
	
	/**
	 * Returns a list of all objects in the collection with the specified type ID.
	 *
	 * @param type_id Scene object type ID.
	 * @return List of scene objects with the specified type ID.
	 */
	const std::list<object_base*>* get_objects(std::size_t type_id) const;

private:
	std::list<object_base*> objects;
	mutable std::unordered_map<std::size_t, std::list<object_base*>> object_map;
};

inline const std::list<object_base*>* collection::get_objects() const
{
	return &objects;
}

inline const std::list<object_base*>* collection::get_objects(std::size_t type_id) const
{
	return &object_map[type_id];
}

} // namespace scene

#endif // ANTKEEPER_SCENE_COLLECTION_HPP

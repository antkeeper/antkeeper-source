/*
 * Copyright (C) 2023  Christopher J. Howard
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

#include <vector>
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
	
	/// Returns all objects in the collection.
	[[nodiscard]] inline const std::vector<object_base*>& get_objects() const noexcept
	{
		return objects;
	}
	
	/**
	 * Returns all objects in the collection with the given type ID.
	 *
	 * @param type_id Scene object type ID.
	 *
	 * @return Scene objects with the given type ID.
	 */
	[[nodiscard]] inline const std::vector<object_base*>& get_objects(std::size_t type_id) const
	{
		return object_map[type_id];
	}

private:
	std::vector<object_base*> objects;
	
	mutable std::unordered_map<std::size_t, std::vector<object_base*>> object_map;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_COLLECTION_HPP

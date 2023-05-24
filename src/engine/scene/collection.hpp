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

#include <engine/scene/object.hpp>
#include <vector>
#include <unordered_map>

namespace scene {

/**
 * Collection of scene objects.
 */
class collection
{
public:
	/// @name Objects
	/// @{
	
	/**
	 * Adds an object to the collection.
	 *
	 * @param object Object to add.
	 */
	void add_object(object_base& object);
	
	/**
	 * Removes an object from the collection.
	 *
	 * @param object Object to remove.
	 */
	void remove_object(const object_base& object);
	
	/// Removes all objects from the collection.
	void remove_objects();
	
	/// Returns all objects in the collection.
	[[nodiscard]] inline const std::vector<object_base*>& get_objects() const noexcept
	{
		return m_objects;
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
		return m_object_map[type_id];
	}
	
	/// @}
	/// @name Settings
	/// @{
	
	/**
	 * Sets the scale of the scene.
	 *
	 * @param scale Ratio of meters to scene units.
	 */
	inline void set_scale(float scale) noexcept
	{
		m_scale = scale;
	}
	
	/// Returns the ratio of meters to scene units.
	[[nodiscard]] inline float get_scale() const noexcept
	{
		return m_scale;
	}
	
	/// @}

private:
	std::vector<object_base*> m_objects;
	mutable std::unordered_map<std::size_t, std::vector<object_base*>> m_object_map;
	float m_scale{1.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_COLLECTION_HPP

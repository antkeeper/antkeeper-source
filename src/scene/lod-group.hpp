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

#ifndef ANTKEEPER_LOD_GROUP_HPP
#define ANTKEEPER_LOD_GROUP_HPP

#include "scene/scene-object.hpp"
#include "geometry/aabb.hpp"
#include <list>
#include <vector>

class camera;

class lod_group: public scene_object<lod_group>
{
public:
	/**
	 * Creates a LOD group.
	 *
	 * @param level_count Number of detail levels in the group.
	 */
	lod_group(std::size_t level_count);
	
	/// Creates a LOD group with one level of detail.
	lod_group();
	
	/**
	 * Resizes the LOD group to accommodate the specified number of detail levels.
	 *
	 * @param level_count Number of desired detail levels in the group.
	 */
	void resize(std::size_t level_count);
	
	/**
	 * Selects the appropriate level of detail for a camera.
	 *
	 * @param camera Camera for which the LOD should be selected.
	 * @return Selected level of detail.
	 */
	std::size_t select_lod(const ::camera& camera) const;
	
	/**
	 * Adds an object to the LOD group.
	 *
	 * @param level Level of detail of the object to add.
	 * @param object Object to add.
	 */
	void add_object(std::size_t level, scene_object_base* object);
	
	/**
	 * Removes an object from the LOD group.
	 *
	 * @param level Level of detail of the object to remove.
	 * @param object Object to remove.
	 */
	void remove_object(std::size_t level, scene_object_base* object);
	
	/**
	 * Removes all objects with the specified level of detail.
	 *
	 * @param level Level of detail of the objects to remove.
	 */
	void remove_objects(std::size_t level);
	
	virtual const bounding_volume<float>& get_bounds() const;
	
	/// Returns the number of detail levels in the group.
	std::size_t get_level_count() const;
	
	/**
	 * Returns a list containing all objects in the LOD group with the specified detail level.
	 *
	 * @param level Level of detail.
	 * @return List of all objects in the group with the specified detail level.
	 */
	const std::list<scene_object_base*>& get_objects(std::size_t level) const;
	
private:
	void update_bounds();
	virtual void transformed();
	
	aabb<float> bounds;
	std::vector<std::list<scene_object_base*>> levels;
};

inline const bounding_volume<float>& lod_group::get_bounds() const
{
	return bounds;
}

inline std::size_t lod_group::get_level_count() const
{
	return levels.size();
}

inline const std::list<scene_object_base*>& lod_group::get_objects(std::size_t level) const
{
	return levels[level];
}

#endif // ANTKEEPER_LOD_GROUP_HPP


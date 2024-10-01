// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_COLLECTION_HPP
#define ANTKEEPER_SCENE_COLLECTION_HPP

#include <engine/scene/object.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace scene {

/// Collection of scene objects.
class collection
{
public:
	/// @name Objects
	/// @{
	
	/// Adds an object to the collection.
	/// @param object Object to add.
	void add_object(object_base& object);
	
	/// Removes an object from the collection.
	/// @param object Object to remove.
	void remove_object(const object_base& object);
	
	/// Removes all objects from the collection.
	void remove_objects();
	
	/// Returns all objects in the collection.
	[[nodiscard]] inline const std::vector<object_base*>& get_objects() const noexcept
	{
		return m_objects;
	}
	
	/// Returns all objects in the collection with the given type ID.
	/// @param type_id Scene object type ID.
	/// @return Scene objects with the given type ID.
	[[nodiscard]] inline const std::vector<object_base*>& get_objects(std::size_t type_id) const
	{
		return m_object_map[type_id];
	}
	
	/// @}
	/// @name Settings
	/// @{
	
	/// Sets the scale of the scene.
	/// @param scale Ratio of meters to scene units.
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
	std::unordered_set<const object_base*> m_object_set;
	mutable std::unordered_map<std::size_t, std::vector<object_base*>> m_object_map;
	float m_scale{1.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_COLLECTION_HPP

/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_SCENE_OBJECT_HPP
#define ANTKEEPER_SCENE_OBJECT_HPP

#include <atomic>
#include <cstdlib>
#include <vmq/vmq.hpp>
#include "animation/tween.hpp"
#include "geometry/bounding-volume.hpp"

using namespace vmq::types;
using vmq::quaternion;
using vmq::transform;

/**
 * Internal base class for scene objects.
 */
class scene_object_base
{
public:
	/// Returns the type ID for this scene_object type.
	virtual const std::size_t get_object_type_id() const = 0;

	/**
	 * Creates a scene object base.
	 */
	scene_object_base();

	/**
	 * Destroys a scene object base.
	 */
	virtual ~scene_object_base() = default;

	/**
	 * Updates all tweens in the scene object.
	 */
	virtual void update_tweens();
	
	/**
	 * Activates or deactivates the scene object.
	 */
	void set_active(bool active);

	/**
	 *
	 */
	void look_at(const float3& position, const float3& target, const float3& up);

	/**
	 * Sets the scene object's transform.
	 */
	void set_transform(const transform<float>& transform);

	/**
	 * Sets the scene object's translation.
	 */
	void set_translation(const float3& translation);
	
	/**
	 * Sets the scene object's rotation.
	 */
	void set_rotation(const quaternion<float>& rotation);

	/**
	 * Sets the scene object's scale.
	 */
	void set_scale(const float3& scale);
	
	/**
	 * Sets a culling mask for the object, which will be used for view-frustum culling instead of the object's bounds.
	 */
	void set_culling_mask(const bounding_volume<float>* culling_mask);
	
	/// Returns whether the scene object is active.
	bool is_active() const;

	/**
	 * Returns the transform.
	 */
	const transform<float>& get_transform() const;

	/**
	 * Returns the transform's translation vector.
	 */
	const float3& get_translation() const;

	/**
	 * Returns the transform's rotation quaternion.
	 */
	const quaternion<float>& get_rotation() const;

	/**
	 * Returns the transform's scale vector.
	 */
	const float3& get_scale() const;

	/**
	 * Returns the transform tween.
	 */
	const tween<transform<float>>& get_transform_tween() const;
	tween<transform<float>>& get_transform_tween();

	/**
	 * Returns the bounds of the object.
	 */
	virtual const bounding_volume<float>& get_bounds() const = 0;
	
	/**
	 * Returns the culling mask of the object.
	 */
	const bounding_volume<float>* get_culling_mask() const;

protected:
	static std::size_t next_object_type_id();

private:
	/**
	 * Called every time the scene object's tranform is changed.
	 */
	virtual void transformed();

	bool active;
	tween<transform<float>> transform;
	const bounding_volume<float>* culling_mask;
};

inline void scene_object_base::set_active(bool active)
{
	this->active = active;
}

inline void scene_object_base::set_transform(const ::transform<float>& transform)
{
	this->transform[1] = transform;
	transformed();
}

inline void scene_object_base::set_translation(const float3& translation)
{
	transform[1].translation = translation;
	transformed();
}

inline void scene_object_base::set_rotation(const quaternion<float>& rotation)
{
	transform[1].rotation = rotation;
	transformed();
}

inline void scene_object_base::set_scale(const float3& scale)
{
	transform[1].scale = scale;
	transformed();
}

inline bool scene_object_base::is_active() const
{
	return active;
}

inline const transform<float>& scene_object_base::get_transform() const
{
	return transform[1];
}

inline const float3& scene_object_base::get_translation() const
{
	return get_transform().translation;
}

inline const quaternion<float>& scene_object_base::get_rotation() const
{
	return get_transform().rotation;
}

inline const float3& scene_object_base::get_scale() const
{
	return get_transform().scale;
}

inline const tween<transform<float>>& scene_object_base::get_transform_tween() const
{
	return transform;
}

inline tween<transform<float>>& scene_object_base::get_transform_tween()
{
	return transform;
}

inline const bounding_volume<float>* scene_object_base::get_culling_mask() const
{
	return culling_mask;
}

/**
 * Abstract base class for lights, cameras, model instances, and other scene objects.
 *
 * @tparam T This should be the same class that's inheriting from the scene object, in order to give it a valid type-specific ID.
 */
template <class T>
class scene_object: public scene_object_base
{
public:
	/// Unique type ID for this scene_object type.
	static const std::atomic<std::size_t> object_type_id;

	virtual const std::size_t get_object_type_id() const final;
};

template <typename T>
const std::atomic<std::size_t> scene_object<T>::object_type_id{scene_object_base::next_object_type_id()};

template <typename T>
inline const std::size_t scene_object<T>::get_object_type_id() const
{
	return object_type_id;
}

#endif // ANTKEEPER_SCENE_OBJECT_HPP


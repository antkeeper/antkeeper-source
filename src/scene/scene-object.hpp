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

#ifndef ANTKEEPER_SCENE_OBJECT_HPP
#define ANTKEEPER_SCENE_OBJECT_HPP

#include "animation/tween.hpp"
#include "geometry/bounding-volume.hpp"
#include "math/vector-type.hpp"
#include "math/quaternion-type.hpp"
#include "math/transform-type.hpp"
#include <atomic>
#include <cstdlib>

/**
 * Internal base class for scene objects.
 */
class scene_object_base
{
public:
	typedef math::vector<float, 3> vector_type;
	typedef math::quaternion<float> quaternion_type;
	typedef math::transform<float> transform_type;
	typedef bounding_volume<float> bounding_volume_type;
	
	/// Returns the type ID for this scene object type.
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
	void look_at(const vector_type& position, const vector_type& target, const vector_type& up);

	/**
	 * Sets the scene object's transform.
	 */
	void set_transform(const transform_type& transform);

	/**
	 * Sets the scene object's translation.
	 */
	void set_translation(const vector_type& translation);
	
	/**
	 * Sets the scene object's rotation.
	 */
	void set_rotation(const quaternion_type& rotation);

	/**
	 * Sets the scene object's scale.
	 */
	void set_scale(const vector_type& scale);
	
	/**
	 * Sets a culling mask for the object, which will be used for view-frustum culling instead of the object's bounds.
	 */
	void set_culling_mask(const bounding_volume_type* culling_mask);
	
	/// Returns whether the scene object is active.
	bool is_active() const;

	/**
	 * Returns the transform.
	 */
	const transform_type& get_transform() const;

	/**
	 * Returns the transform's translation vector.
	 */
	const vector_type& get_translation() const;

	/**
	 * Returns the transform's rotation quaternion.
	 */
	const quaternion_type& get_rotation() const;

	/**
	 * Returns the transform's scale vector.
	 */
	const vector_type& get_scale() const;

	/**
	 * Returns the transform tween.
	 */
	const tween<transform_type>& get_transform_tween() const;
	tween<transform_type>& get_transform_tween();

	/**
	 * Returns the bounds of the object.
	 */
	virtual const bounding_volume_type& get_bounds() const = 0;
	
	/**
	 * Returns the culling mask of the object.
	 */
	const bounding_volume_type* get_culling_mask() const;

protected:
	static std::size_t next_object_type_id();

private:
	/// Interpolates between two transforms.
	static transform_type interpolate_transforms(const transform_type& x, const transform_type& y, float a);
	
	/**
	 * Called every time the scene object's tranform is changed.
	 */
	virtual void transformed();

	bool active;
	tween<transform_type> transform;
	const bounding_volume_type* culling_mask;
};

inline void scene_object_base::set_active(bool active)
{
	this->active = active;
}

inline void scene_object_base::set_transform(const transform_type& transform)
{
	this->transform[1] = transform;
	transformed();
}

inline void scene_object_base::set_translation(const vector_type& translation)
{
	transform[1].translation = translation;
	transformed();
}

inline void scene_object_base::set_rotation(const quaternion_type& rotation)
{
	transform[1].rotation = rotation;
	transformed();
}

inline void scene_object_base::set_scale(const vector_type& scale)
{
	transform[1].scale = scale;
	transformed();
}

inline bool scene_object_base::is_active() const
{
	return active;
}

inline const typename scene_object_base::transform_type& scene_object_base::get_transform() const
{
	return transform[1];
}

inline const typename scene_object_base::vector_type& scene_object_base::get_translation() const
{
	return get_transform().translation;
}

inline const typename scene_object_base::quaternion_type& scene_object_base::get_rotation() const
{
	return get_transform().rotation;
}

inline const typename scene_object_base::vector_type& scene_object_base::get_scale() const
{
	return get_transform().scale;
}

inline const tween<typename scene_object_base::transform_type>& scene_object_base::get_transform_tween() const
{
	return transform;
}

inline tween<typename scene_object_base::transform_type>& scene_object_base::get_transform_tween()
{
	return transform;
}

inline const typename scene_object_base::bounding_volume_type* scene_object_base::get_culling_mask() const
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
	/// Unique type ID for this scene object type.
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


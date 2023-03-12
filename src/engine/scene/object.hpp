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

#ifndef ANTKEEPER_SCENE_OBJECT_HPP
#define ANTKEEPER_SCENE_OBJECT_HPP

#include <engine/animation/tween.hpp>
#include <engine/geom/bounding-volume.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/transform-type.hpp>
#include <engine/render/context.hpp>
#include <atomic>
#include <cstddef>

namespace scene {

/**
 * Internal base class for scene objects.
 */
class object_base
{
public:
	typedef math::vector<float, 3> vector_type;
	typedef math::quaternion<float> quaternion_type;
	typedef math::transform<float> transform_type;
	typedef geom::bounding_volume<float> bounding_volume_type;
	
	/// Returns the type ID for this scene object type.
	virtual const std::size_t get_object_type_id() const noexcept = 0;

	/**
	 * Creates a scene object base.
	 */
	object_base();
	
	/**
	 * Adds render operations to a render context.
	 *
	 * @param ctx Render context.
	 */
	inline virtual void render(render::context& ctx) const {}

	/**
	 * Updates all tweens in the scene object.
	 */
	virtual void update_tweens();
	
	/**
	 * Activates or deactivates the scene object.
	 */
	inline void set_active(bool active) noexcept
	{
		this->active = active;
	}

	/**
	 *
	 */
	void look_at(const vector_type& position, const vector_type& target, const vector_type& up);

	/**
	 * Sets the scene object's transform.
	 */
	inline void set_transform(const transform_type& transform)
	{
		this->transform[1] = transform;
		transformed();
	}

	/**
	 * Sets the scene object's translation.
	 */
	inline void set_translation(const vector_type& translation)
	{
		transform[1].translation = translation;
		transformed();
	}
	
	/**
	 * Sets the scene object's rotation.
	 */
	inline void set_rotation(const quaternion_type& rotation)
	{
		transform[1].rotation = rotation;
		transformed();
	}

	/**
	 * Sets the scene object's scale.
	 */
	inline void set_scale(const vector_type& scale)
	{
		transform[1].scale = scale;
		transformed();
	}
	
	/**
	 * Sets a culling mask for the object, which will be used for view-frustum culling instead of the object's bounds.
	 */
	inline void set_culling_mask(const bounding_volume_type* culling_mask) noexcept
	{
		this->culling_mask = culling_mask;
	}
	
	/// Returns whether the scene object is active.
	[[nodiscard]] inline bool is_active() const noexcept
	{
		return active;
	}

	/**
	 * Returns the transform.
	 */
	[[nodiscard]] inline const transform_type& get_transform() const noexcept
	{
		return transform[1];
	}

	/**
	 * Returns the transform's translation vector.
	 */
	[[nodiscard]] inline const vector_type& get_translation() const noexcept
	{
		return transform[1].translation;
	}

	/**
	 * Returns the transform's rotation quaternion.
	 */
	[[nodiscard]] inline const quaternion_type& get_rotation() const noexcept
	{
		return transform[1].rotation;
	}

	/**
	 * Returns the transform's scale vector.
	 */
	[[nodiscard]] inline const vector_type& get_scale() const noexcept
	{
		return transform[1].scale;
	}

	/**
	 * Returns the transform tween.
	 */
	/// @{
	[[nodiscard]] inline const tween<transform_type>& get_transform_tween() const noexcept
	{
		return transform;
	}
	[[nodiscard]] inline tween<transform_type>& get_transform_tween() noexcept
	{
		return transform;
	}
	/// @}

	/**
	 * Returns the local-space (untransformed) bounds of the object.
	 */
	[[nodiscard]] virtual const bounding_volume_type& get_local_bounds() const = 0;
	
	/**
	 * Returns the world-space (transformed) bounds of the object.
	 */
	[[nodiscard]] virtual const bounding_volume_type& get_world_bounds() const = 0;
	
	/**
	 * Returns the culling mask of the object.
	 */
	[[nodiscard]] inline const bounding_volume_type* get_culling_mask() const noexcept
	{
		return culling_mask;
	}

protected:
	static std::size_t next_object_type_id();

private:
	/// Interpolates between two transforms.
	static transform_type interpolate_transforms(const transform_type& x, const transform_type& y, float a);
	
	/**
	 * Called every time the scene object's tranform is changed.
	 */
	virtual void transformed();

	bool active{true};
	tween<transform_type> transform;
	const bounding_volume_type* culling_mask{nullptr};
};

/**
 * Abstract base class for lights, cameras, model instances, and other scene objects.
 *
 * @tparam T This should be the same class that's inheriting from the scene object, in order to give it a valid type-specific ID.
 */
template <class T>
class object: public object_base
{
public:
	/// Unique type ID for this scene object type.
	static const std::atomic<std::size_t> object_type_id;
	
	inline const std::size_t get_object_type_id() const noexcept final
	{
		return object_type_id;
	}
};

template <typename T>
const std::atomic<std::size_t> object<T>::object_type_id{object_base::next_object_type_id()};

} // namespace scene

#endif // ANTKEEPER_SCENE_OBJECT_HPP

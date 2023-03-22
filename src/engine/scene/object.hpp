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

#include <engine/geom/primitives/box.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/transform.hpp>
#include <engine/render/context.hpp>
#include <atomic>
#include <cstdint>

namespace scene {

/**
 * Abstract base class for scene objects.
 */
class object_base
{
public:
	using vector_type = math::vector<float, 3>;
	using quaternion_type = math::quaternion<float>;
	using transform_type = math::transform<float>;
	using aabb_type = geom::box<float>;
	
	/// Returns the type ID for this scene object type.
	virtual const std::size_t get_object_type_id() const noexcept = 0;
	
	/**
	 * Adds render operations to a render context.
	 *
	 * @param ctx Render context.
	 */
	inline virtual void render(render::context& ctx) const {}
	
	/**
	 *
	 */
	void look_at(const vector_type& position, const vector_type& target, const vector_type& up);

	/**
	 * Sets the scene object's transform.
	 */
	inline void set_transform(const transform_type& transform)
	{
		m_transform = transform;
		transformed();
	}

	/**
	 * Sets the scene object's translation.
	 */
	inline void set_translation(const vector_type& translation)
	{
		m_transform.translation = translation;
		transformed();
	}
	
	/**
	 * Sets the scene object's rotation.
	 */
	inline void set_rotation(const quaternion_type& rotation)
	{
		m_transform.rotation = rotation;
		transformed();
	}

	/**
	 * Sets the scene object's scale.
	 */
	inline void set_scale(const vector_type& scale)
	{
		m_transform.scale = scale;
		transformed();
	}

	/**
	 * Returns the transform.
	 */
	[[nodiscard]] inline const transform_type& get_transform() const noexcept
	{
		return m_transform;
	}

	/**
	 * Returns the transform's translation vector.
	 */
	[[nodiscard]] inline const vector_type& get_translation() const noexcept
	{
		return m_transform.translation;
	}

	/**
	 * Returns the transform's rotation quaternion.
	 */
	[[nodiscard]] inline const quaternion_type& get_rotation() const noexcept
	{
		return m_transform.rotation;
	}

	/**
	 * Returns the transform's scale vector.
	 */
	[[nodiscard]] inline const vector_type& get_scale() const noexcept
	{
		return m_transform.scale;
	}
	
	/**
	 * Returns the bounds of the object.
	 */
	[[nodiscard]] virtual const aabb_type& get_bounds() const noexcept = 0;

protected:
	static std::size_t next_object_type_id();

private:
	/// Interpolates between two transforms.
	static transform_type interpolate_transforms(const transform_type& x, const transform_type& y, float a);
	
	/**
	 * Called every time the scene object's tranform is changed.
	 */
	inline virtual void transformed() {}
	
	transform_type m_transform{transform_type::identity()};
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

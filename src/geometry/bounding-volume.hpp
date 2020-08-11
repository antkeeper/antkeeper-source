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

#ifndef ANTKEEPER_BOUNDING_VOLUME_HPP
#define ANTKEEPER_BOUNDING_VOLUME_HPP

#include "math/math.hpp"
#include <stdexcept>

using math::vector;

template <class T>
struct sphere;
template <class T>
struct aabb;

/// Enumerates bounding volume types.
enum class bounding_volume_type
{
	aabb, ///< Indicates the bounding volume is an axis-aligned bounding box.
	sphere, ///< Indicates the bounding volume is a sphere.
	convex_hull ///< Indicates the bounding volume is a convex hull.
};

/**
 * Abstract base class for bounding volumes.
 *
 * @tparam T Scalar type.
 */
template <class T>
class bounding_volume
{
public:
	/// Returns the enumerated type of this bounding volume.
	virtual bounding_volume_type get_bounding_volume_type() const = 0;
	
	/// Tests for intersection between this bounding volume and a bounding sphere.
	virtual bool intersects(const sphere<T>& sphere) const = 0;
	
	/// Tests for intersection between this bounding volume and an axis-aligned bounding box.
	virtual bool intersects(const aabb<T>& aabb) const = 0;
	
	/// Tests whether this bounding volume contains a sphere.
	virtual bool contains(const sphere<T>& sphere) const = 0;
	
	/// Tests whether this bounding volume contains an axis-aligned bounding box.
	virtual bool contains(const aabb<T>& aabb) const = 0;
	
	/// Tests whether this bounding volume contains a point.
	virtual bool contains(const vector<T, 3>& point) const = 0;
	
	/// Tests for intersection between this bounding volume and another bounding volume.
	bool intersects(const bounding_volume& volume) const;
};

/// Tests for intersection between this bounding volume and another bounding volume.
template <class T>
bool bounding_volume<T>::intersects(const bounding_volume& volume) const
{
	const bounding_volume_type type = volume.get_bounding_volume_type();
	switch (type)
	{
		case bounding_volume_type::sphere:
			return intersects(static_cast<const sphere<T>&>(volume));
			break;
		
		case bounding_volume_type::aabb:
			return intersects(static_cast<const aabb<T>&>(volume));
			break;
		
		default:
			throw std::runtime_error("unimplemented");
			break;
	}
}

#endif // ANTKEEPER_BOUNDING_VOLUME_HPP

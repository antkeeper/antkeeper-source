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

#ifndef ANTKEEPER_CONVEX_HULL_HPP
#define ANTKEEPER_CONVEX_HULL_HPP

#include <cstdlib>
#include <vector>
#include "bounding-volume.hpp"
#include "plane.hpp"
#include "sphere.hpp"
#include "aabb.hpp"

/**
 * A plane-bounded convex hull.
 */
template <class T>
struct convex_hull: public bounding_volume<T>
{
	/// Vector of planes with descibe the bounds of the convex hull.
	std::vector<plane<T>> planes;
	
	/**
	 * Creates a convex hull.
	 *
	 * @param size Number of planes the convex hull should accommodate.
	 */
	convex_hull(std::size_t size);
	
	/// Creates a convex hull.
	convex_hull();
	
	virtual bounding_volume_type get_bounding_volume_type() const;
	virtual bool intersects(const sphere<T>& sphere) const;
	virtual bool intersects(const aabb<T>& aabb) const;
	virtual bool contains(const sphere<T>& sphere) const;
	virtual bool contains(const aabb<T>& aabb) const;
	virtual bool contains(const vector<T, 3>& point) const;
};

template <class T>
convex_hull<T>::convex_hull(std::size_t size):
	planes(size, plane<T>())
{}

template <class T>
convex_hull<T>::convex_hull()
{}

template <class T>
inline bounding_volume_type convex_hull<T>::get_bounding_volume_type() const
{
	return bounding_volume_type::convex_hull;
}

template <class T>
bool convex_hull<T>::intersects(const sphere<T>& sphere) const
{
	for (const plane<T>& plane: planes)
		if (signed_distance(plane, sphere.center) < -sphere.radius)
			return false;
	return true;
}

template <class T>
bool convex_hull<T>::intersects(const aabb<T>& aabb) const
{
	vector<T, 3> pv;
	for (const plane<T>& plane: planes)
	{	
		pv.x = (plane.normal.x > T(0)) ? aabb.max_point.x : aabb.min_point.x;
		pv.y = (plane.normal.y > T(0)) ? aabb.max_point.y : aabb.min_point.y;
		pv.z = (plane.normal.z > T(0)) ? aabb.max_point.z : aabb.min_point.z;
		if (signed_distance(plane, pv) < T(0))
			return false;
	}
	
	return true;
}

template <class T>
bool convex_hull<T>::contains(const sphere<T>& sphere) const
{
	for (const plane<T>& plane: planes)
		if (signed_distance(plane, sphere.center) < sphere.radius)
			return false;
	return true;
}

template <class T>
bool convex_hull<T>::contains(const aabb<T>& aabb) const
{
	vector<T, 3> pv;
	vector<T, 3> nv;
	
	for (const plane<T>& plane: planes)
	{
		pv.x = (plane.normal.x > T(0)) ? aabb.max_point.x : aabb.min_point.x;
		pv.y = (plane.normal.y > T(0)) ? aabb.max_point.y : aabb.min_point.y;
		pv.z = (plane.normal.z > T(0)) ? aabb.max_point.z : aabb.min_point.z;
		nv.x = (plane.normal.x < T(0)) ? aabb.max_point.x : aabb.min_point.x;
		nv.y = (plane.normal.y < T(0)) ? aabb.max_point.y : aabb.min_point.y;
		nv.z = (plane.normal.z < T(0)) ? aabb.max_point.z : aabb.min_point.z;
		
		if (signed_distance(plane, pv) < T(0) || signed_distance(plane, nv) < T(0))
			return false;
	}
	
	return true;
}

template <class T>
bool convex_hull<T>::contains(const vector<T, 3>& point) const
{
	for (const plane<T>& plane: planes)
		if (signed_distance(plane, point) < T(0))
			return false;

	return true;
}

#endif // ANTKEEPER_CONVEX_HULL_HPP


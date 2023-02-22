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

#ifndef ANTKEEPER_GEOM_SPHERE_HPP
#define ANTKEEPER_GEOM_SPHERE_HPP

#include <engine/geom/bounding-volume.hpp>
#include <engine/geom/aabb.hpp>
#include <engine/math/vector.hpp>
#include <algorithm>

namespace geom {

/**
 * Bounding sphere.
 */
template <class T>
struct sphere: public bounding_volume<T>
{
	typedef math::vector<T, 3> vector_type;
	
	vector_type center;
	T radius;
	
	sphere(const vector_type& center, T radius);
	sphere();
	
	virtual bounding_volume_type get_bounding_volume_type() const;
	virtual bool intersects(const sphere<T>& sphere) const;
	virtual bool intersects(const aabb<T>& aabb) const;
	virtual bool contains(const sphere<T>& sphere) const;
	virtual bool contains(const aabb<T>& aabb) const;
	virtual bool contains(const vector_type& point) const;
};

template <class T>
sphere<T>::sphere(const vector_type& center, T radius):
	center(center),
	radius(radius)
{}

template <class T>
sphere<T>::sphere()
{}

template <class T>
inline bounding_volume_type sphere<T>::get_bounding_volume_type() const
{
	return bounding_volume_type::sphere;
}

template <class T>
bool sphere<T>::intersects(const sphere<T>& sphere) const
{
	vector_type d = center - sphere.center;
	T r = radius + sphere.radius;
	return (math::dot(d, d) <= r * r);
}

template <class T>
bool sphere<T>::intersects(const aabb<T>& aabb) const
{
	return aabb.intersects(*this);
}

template <class T>
bool sphere<T>::contains(const sphere<T>& sphere) const
{
	T containment_radius = radius - sphere.radius;
	if (containment_radius < T(0))
		return false;
	
	vector_type d = center - sphere.center;
	return (math::dot(d, d) <= containment_radius * containment_radius);
}

template <class T>
bool sphere<T>::contains(const aabb<T>& aabb) const
{
	T distance = T(0);
	
	vector_type a = center - aabb.min_point;
	vector_type b = center - aabb.max_point;
	
	distance += std::max(a.x() * a.x(), b.x() * b.x());
	distance += std::max(a.y() * a.y(), b.y() * b.y());
	distance += std::max(a.z() * a.z(), b.z() * b.z());
	
	return (distance <= radius * radius);
}

template <class T>
bool sphere<T>::contains(const vector_type& point) const
{
	vector_type d = center - point;
	return (math::dot(d, d) <= radius * radius);
}

} // namespace geom

#endif // ANTKEEPER_GEOM_SPHERE_HPP

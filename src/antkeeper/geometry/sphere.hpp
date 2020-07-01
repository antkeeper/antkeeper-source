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

#ifndef ANTKEEPER_SPHERE_HPP
#define ANTKEEPER_SPHERE_HPP

#include "bounding-volume.hpp"
#include "aabb.hpp"
#include <vmq/vmq.hpp>
#include <algorithm>

using vmq::vector;
using namespace vmq::operators;

template <class T>
struct sphere: public bounding_volume<T>
{
	vector<T, 3> center;
	T radius;
	
	sphere(const vector<T, 3>& center, T radius);
	sphere();
	
	virtual bounding_volume_type get_bounding_volume_type() const;
	virtual bool intersects(const sphere<T>& sphere) const;
	virtual bool intersects(const aabb<T>& aabb) const;
	virtual bool contains(const sphere<T>& sphere) const;
	virtual bool contains(const aabb<T>& aabb) const;
	virtual bool contains(const vector<T, 3>& point) const;
};

template <class T>
sphere<T>::sphere(const vector<T, 3>& center, T radius):
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
	vector<T, 3> d = center - sphere.center;
	float r = radius + sphere.radius;
	return (vmq::dot(d, d) <= r * r);
}

template <class T>
bool sphere<T>::intersects(const aabb<T>& aabb) const
{
	return aabb.intersects(*this);
}

template <class T>
bool sphere<T>::contains(const sphere<T>& sphere) const
{
	float containment_radius = radius - sphere.radius;
	if (containment_radius < T(0))
		return false;
	
	vector<T, 3> d = center - sphere.center;
	return (vmq::dot(d, d) <= containment_radius * containment_radius);
}

template <class T>
bool sphere<T>::contains(const aabb<T>& aabb) const
{
	T distance = T(0);
	
	vector<T, 3> a = center - aabb.min_point;
	vector<T, 3> b = center - aabb.max_point;
	
	distance += std::max(a.x * a.x, b.x * b.x);
	distance += std::max(a.y * a.y, b.y * b.y);
	distance += std::max(a.z * a.z, b.z * b.z);
	
	return (distance <= radius * radius);
}

template <class T>
bool sphere<T>::contains(const vector<T, 3>& point) const
{
	vector<T, 3> d = center - point;
	return (vmq::dot(d, d) <= radius * radius);
}

#endif // ANTKEEPER_SPHERE_HPP


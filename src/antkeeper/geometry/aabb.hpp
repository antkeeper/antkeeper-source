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

#ifndef ANTKEEPER_AABB_HPP
#define ANTKEEPER_AABB_HPP

#include "bounding-volume.hpp"
#include "sphere.hpp"
#include <vmq/vmq.hpp>
#include <array>
#include <limits>

using vmq::vector;
using vmq::matrix;
using vmq::transform;
using namespace vmq::operators;

template <class T>
struct aabb: public bounding_volume<T>
{
	vector<T, 3> min_point;
	vector<T, 3> max_point;
	
	/**
	 * Transforms an AABB.
	 *
	 * @param a AABB to be transformed.
	 * @param t Transform by which the AABB should be transformed.
	 * @return Transformed AABB.
	 */
	static aabb transform(const aabb& a, const ::transform<T>& t);
	
	/**
	 * Transforms an AABB.
	 *
	 * @param a AABB to be transformed.
	 * @param m Matrix by which the AABB should be transformed.
	 * @return Transformed AABB.
	 */
	static aabb transform(const aabb& a, const matrix<T, 4, 4>& m);
	
	aabb(const vector<T, 3>& min_point, const vector<T, 3>& max_point);
	aabb();
	
	virtual bounding_volume_type get_bounding_volume_type() const;
	virtual bool intersects(const sphere<T>& sphere) const;
	virtual bool intersects(const aabb<T>& aabb) const;
	virtual bool contains(const sphere<T>& sphere) const;
	virtual bool contains(const aabb<T>& aabb) const;
	virtual bool contains(const vector<T, 3>& point) const;

	/**
	 * Returns the position of the specified corner.
	 *
	 * @param index Index of a corner.
	 * @return Position of the specified corner.
	 */
	vector<T, 3> corner(int index) const noexcept;
};

template <class T>
aabb<T> aabb<T>::transform(const aabb& a, const ::transform<T>& t)
{
	vector<T, 3> min_point = {std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity()};
	vector<T, 3> max_point = {-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity()};
	
	for (std::size_t i = 0; i < 8; ++i)
	{
		vector<T, 3> transformed_corner = vmq::mul(t, a.corner(i));

		for (std::size_t j = 0; j < 3; ++j)
		{
			min_point[j] = std::min<T>(min_point[j], transformed_corner[j]);
			max_point[j] = std::max<T>(max_point[j], transformed_corner[j]);
		}
	}

	return {min_point, max_point};
}

template <class T>
aabb<T> aabb<T>::transform(const aabb& a, const matrix<T, 4, 4>& m)
{
	vector<T, 3> min_point = {std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity()};
	vector<T, 3> max_point = {-std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity()};
	
	for (std::size_t i = 0; i < 8; ++i)
	{
		vector<T, 3> corner = a.corner(i);
		vector<T, 4> transformed_corner = vmq::mul(m, vector<T, 4>{corner.x, corner.y, corner.z, T(1)});

		for (std::size_t j = 0; j < 3; ++j)
		{
			min_point[j] = std::min<T>(min_point[j], transformed_corner[j]);
			max_point[j] = std::max<T>(max_point[j], transformed_corner[j]);
		}
	}

	return {min_point, max_point};
}

template <class T>
aabb<T>::aabb(const vector<T, 3>& min_point, const vector<T, 3>& max_point):
	min_point(min_point),
	max_point(max_point)
{}

template <class T>
aabb<T>::aabb()
{}

template <class T>
inline bounding_volume_type aabb<T>::get_bounding_volume_type() const
{
	return bounding_volume_type::aabb;
}

template <class T>
bool aabb<T>::intersects(const sphere<T>& sphere) const
{
	const vector<T, 3> radius_vector = {sphere.radius, sphere.radius, sphere.radius};
	return aabb<T>(min_point - radius_vector, max_point + radius_vector).contains(sphere.center);
}

template <class T>
bool aabb<T>::intersects(const aabb<T>& aabb) const
{
	if (max_point.x < aabb.min_point.x || min_point.x > aabb.max_point.x)
		return false;
	if (max_point.y < aabb.min_point.y || min_point.y > aabb.max_point.y)
		return false;
	if (max_point.z < aabb.min_point.z || min_point.z > aabb.max_point.z)
		return false;
	return true;
}

template <class T>
bool aabb<T>::contains(const sphere<T>& sphere) const
{
	if (sphere.center.x - sphere.radius < min_point.x || sphere.center.x + sphere.radius > max_point.x)
		return false;
	if (sphere.center.y - sphere.radius < min_point.y || sphere.center.y + sphere.radius > max_point.y)
		return false;
	if (sphere.center.z - sphere.radius < min_point.z || sphere.center.z + sphere.radius > max_point.z)
		return false;
	return true;
}

template <class T>
bool aabb<T>::contains(const aabb<T>& aabb) const
{
	if (aabb.min_point.x < min_point.x || aabb.max_point.x > max_point.x)
		return false;
	if (aabb.min_point.y < min_point.y || aabb.max_point.y > max_point.y)
		return false;
	if (aabb.min_point.z < min_point.z || aabb.max_point.z > max_point.z)
		return false;
	return true;
}

template <class T>
bool aabb<T>::contains(const vector<T, 3>& point) const
{
	if (point.x < min_point.x || point.x > max_point.x)
		return false;
	if (point.y < min_point.y || point.y > max_point.y)
		return false;
	if (point.z < min_point.z || point.z > max_point.z)
		return false;
	return true;
}

template <class T>
vector<T, 3> aabb<T>::corner(int index) const noexcept
{
	return
		{
			((index >> 2) & 1) ? max_point[0] : min_point[0],
			((index >> 1) & 1) ? max_point[1] : min_point[1],
			((index >> 0) & 1) ? max_point[2] : min_point[2]
		};
}

#endif // ANTKEEPER_AABB_HPP


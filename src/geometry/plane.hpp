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

#ifndef ANTKEEPER_PLANE_HPP
#define ANTKEEPER_PLANE_HPP

#include "math/math.hpp"

template <class T>
struct plane
{
	typedef math::vector<T, 3> vector_type;
	
	/// Plane normal vector.
	vector_type normal;
	
	/// Plane distance.
	T distance;
	
	/**
	 * Creates a plane given a normal vector and distance.
	 */
	plane(const vector_type& normal, T distance);
	
	/**
	 * Creates a plane given a normal vector and offset vector.
	 */
	plane(const vector_type& normal, const vector_type& offset);
	
	/**
	 * Creates a plane given three points.
	 */
	plane(const vector_type& a, const vector_type& b, const vector_type& c);
	
	/**
	 * Creates a plane given its coefficients.
	 *
	 * @param coefficients Vector containing the plane coefficients, A, B, C and D, as x, y, z, and w, respectively.
	 */
	plane(const math::vector<T, 4>& coefficients);
	
	/// Creates an uninitialized plane.
	plane() = default;
	
	/**
	 * Calculates the signed distance between a plane and a vector.
	 *
	 * @param v Vector.
	 * @return Signed distance between the plane and vector.
	 */
	T signed_distance(const vector_type& v) const;
	
	/**
	 * Calculates the point of intersection between three planes.
	 */
	static vector_type intersection(const plane& p0, const plane& p1, const plane& p2);
};

template <class T>
inline plane<T>::plane(const vector_type& normal, T distance):
	normal(normal),
	distance(distance)
{}

template <class T>
plane<T>::plane(const vector_type& normal, const vector_type& offset):
	normal(normal),
	distance(-math::dot(normal, offset))
{}

template <class T>
plane<T>::plane(const vector_type& a, const vector_type& b, const vector_type& c)
{
	normal = math::normalize(math::cross(c - b, a - b));
	distance = -(math::dot(normal, b));
}

template <class T>
plane<T>::plane(const math::vector<T, 4>& coefficients)
{
	const vector_type abc = math::resize<3>(coefficients);
	const float inverse_length = T(1) / math::length(abc);
	
	normal = abc * inverse_length;
	distance = coefficients[3] * inverse_length;
}

template <class T>
inline T plane<T>::signed_distance(const vector_type& v) const
{
	return distance + math::dot(normal, v);
}

template <class T>
typename plane<T>::vector_type plane<T>::intersection(const plane& p0, const plane& p1, const plane& p2)
{
	return -(p0.distance * math::cross(p1.normal, p2.normal) + p1.distance * math::cross(p2.normal, p0.normal) + p2.distance * math::cross(p0.normal, p1.normal)) / math::dot(p0.normal, math::cross(p1.normal, p2.normal));
}

#endif // ANTKEEPER_PLANE_HPP

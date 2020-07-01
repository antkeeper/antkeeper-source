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

#ifndef ANTKEEPER_PLANE_HPP
#define ANTKEEPER_PLANE_HPP

#include <vmq/vmq.hpp>

using vmq::vector;
using namespace vmq::operators;

template <class T>
struct plane
{
	vector<T, 3> normal;
	T distance;
	
	/**
	 * Creates a plane given a normal vector and distance.
	 */
	plane(const vector<T, 3>& normal, T distance);
	
	/**
	 * Creates a plane given a normal vector and offset vector.
	 */
	plane(const vector<T, 3>& normal, const vector<T, 3>& offset);
	
	/**
	 * Creates a plane given three points.
	 */
	plane(const vector<T, 3>& a, const vector<T, 3>& b, const vector<T, 3>& c);
	
	/**
	 * Creates a plane given its coefficients.
	 *
	 * @param coefficients Vector containing the plane coefficients, A, B, C and D, as x, y, z, and w, respectively.
	 */
	plane(const vector<T, 4>& coefficients);
	
	/// Creates an uninitialized plane.
	plane() = default;
};

template <class T>
inline plane<T>::plane(const vector<T, 3>& normal, T distance):
	normal(normal),
	distance(distance)
{}

template <class T>
plane<T>::plane(const vector<T, 3>& normal, const vector<T, 3>& offset):
	normal(normal),
	distance(-vmq::dot(normal, offset))
{}

template <class T>
plane<T>::plane(const vector<T, 3>& a, const vector<T, 3>& b, const vector<T, 3>& c)
{
	normal = vmq::normalize(vmq::cross(c - b, a - b));
	distance = -(vmq::dot(normal, b));
}

template <class T>
plane<T>::plane(const vector<T, 4>& coefficients)
{
	const vector<T, 3> abc = vmq::resize<3>(coefficients);
	const float inverse_length = T(1) / vmq::length(abc);
	
	normal = abc * inverse_length;
	distance = coefficients[3] * inverse_length;
}

/**
 * Calculates the signed distance between a plane and a vector.
 *
 * @param p Plane.
 * @param v Vector.
 * @return Signed distance between the plane and a vector.
 */
template <class T>
inline T signed_distance(const plane<T>& p, const vector<T, 3>& v)
{
	return p.distance + vmq::dot(p.normal, v);
}

/**
 * Calculates the point of intersection between three planes.
 */
template <class T>
vector<T, 3> intersection(const plane<T>& p0, const plane<T>& p1, const plane<T>& p2)
{
	return -(p0.distance * vmq::cross(p1.normal, p2.normal) + p1.distance * vmq::cross(p2.normal, p0.normal) + p2.distance * vmq::cross(p0.normal, p1.normal)) / vmq::dot(p0.normal, vmq::cross(p1.normal, p2.normal));
}

#endif // ANTKEEPER_PLANE_HPP


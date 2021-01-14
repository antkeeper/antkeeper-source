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

#ifndef ANTKEEPER_GEOM_RAY_HPP
#define ANTKEEPER_GEOM_RAY_HPP

#include "math/math.hpp"

namespace geom {

template <class T>
struct ray
{
	typedef math::vector<T, 3> vector_type;
	
	/// Origin of the ray.
	vector_type origin;
	
	/// Normalized direction vector of the ray.
	vector_type direction;

	/**
	 * Extrapolates from the ray origin along the ray direction vector.
	 *
	 * @param distance Distance to extrapolate.
	 * @return Extrapolated coordinates.
	 */
	vector_type extrapolate(T distance) const;
};

template <class T>
inline typename ray<T>::vector_type ray<T>::extrapolate(T distance) const
{
	return origin + direction * distance;
}

} // namespace geom

#endif // ANTKEEPER_GEOM_RAY_HPP


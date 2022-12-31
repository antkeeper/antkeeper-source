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

#ifndef ANTKEEPER_GEOM_PRIMITIVE_RAY_HPP
#define ANTKEEPER_GEOM_PRIMITIVE_RAY_HPP

#include "math/vector.hpp"

namespace geom {
namespace primitive {

/**
 * Half of a line proceeding from an initial point.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct ray
{
	typedef math::vector<T, N> vector_type;
	
	/// Ray origin position.
	vector_type origin;
	
	/// Ray direction vector.
	vector_type direction;
	
	/**
	 * Extrapolates from the ray origin along the ray direction vector.
	 *
	 * @param distance Signed extrapolation distance.
	 *
	 * @return Extrapolated coordinates.
	 */
	constexpr vector_type extrapolate(T distance) const noexcept
	{
		return origin + direction * distance;
	}
};

} // namespace primitive
} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVE_RAY_HPP

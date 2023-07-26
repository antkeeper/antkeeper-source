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

#ifndef ANTKEEPER_GEOM_PRIMITIVES_TRIANGLE_HPP
#define ANTKEEPER_GEOM_PRIMITIVES_TRIANGLE_HPP

#include <engine/math/vector.hpp>

namespace geom {
namespace primitives {

/**
 * *n*-dimensional triangle.
 *
 * @tparam T Real type.
 * @tparam N Number of dimensions.
 */
template <class T, std::size_t N>
struct triangle
{
	/// Vector type.
	using vector_type = math::vector<T, N>;
	
	/// First point.
	vector_type a;
	
	/// Second point.
	vector_type b;
	
	/// Third point.
	vector_type c;
};

} // namespace primitives

using namespace primitives;

} // namespace geom

#endif // ANTKEEPER_GEOM_PRIMITIVES_TRIANGLE_HPP

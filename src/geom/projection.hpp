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

#ifndef ANTKEEPER_GEOM_PROJECTION_HPP
#define ANTKEEPER_GEOM_PROJECTION_HPP

#include "math/vector.hpp"

namespace geom {

template <class T>
math::vector<T, 3> project_on_plane(const math::vector<T, 3>& v, const math::vector<T, 3>& p, const math::vector<T, 3>& n)
{
	return v - n * math::dot(v - p, n);
}

} // namespace geom

#endif // ANTKEEPER_GEOM_PROJECTION_HPP

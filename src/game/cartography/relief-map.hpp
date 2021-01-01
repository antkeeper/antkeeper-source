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

#ifndef ANTKEEPER_RELIEF_MAP_HPP
#define ANTKEEPER_RELIEF_MAP_HPP

#include "geometry/mesh.hpp"
#include <functional>

namespace cart
{

/**
 * Generates a relief map mesh given an elevation function.
 *
 * @param function Function which returns an elevation given UV coordinates on a unit plane.
 * @param subdivisions Number of lines of longitude. Minimum value is 3.
 */
mesh* map_elevation(const std::function<float(float, float)>& function, float scale, std::size_t subdivisions);

} // namespace cart

#endif // ANTKEEPER_RELIEF_MAP_HPP

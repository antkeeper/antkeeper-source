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

#ifndef ANTKEEPER_GEOM_QUADTREE_HPP
#define ANTKEEPER_GEOM_QUADTREE_HPP

#include "geom/hyperoctree.hpp"

namespace geom {

/// A quadtree, or 2-dimensional hyperoctree.
template <class T, std::size_t D>
using quadtree = hyperoctree<T, 2, D>;

/// Quadtree with an 8-bit node type (2 depth levels).
typedef quadtree<std::uint8_t, 1> quadtree8;

/// Quadtree with a 16-bit node type (6 depth levels).
typedef quadtree<std::uint16_t, 5> quadtree16;

/// Quadtree with a 32-bit node type (13 depth levels).
typedef quadtree<std::uint32_t, 12> quadtree32;

/// Quadtree with a 64-bit node type (29 depth levels).
typedef quadtree<std::uint64_t, 28> quadtree64;

} // namespace geom

#endif // ANTKEEPER_GEOM_QUADTREE_HPP

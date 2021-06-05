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
template <std::size_t D, class T>
using quadtree = hyperoctree<2, D, T>;

/// Quadtree with an 8-bit node type (2 depth levels).
typedef quadtree<1, std::uint8_t> quadtree8;

/// Quadtree with a 16-bit node type (6 depth levels).
typedef quadtree<5, std::uint16_t> quadtree16;

/// Quadtree with a 32-bit node type (13 depth levels).
typedef quadtree<12, std::uint32_t> quadtree32;

/// Quadtree with a 64-bit node type (29 depth levels).
typedef quadtree<28, std::uint64_t> quadtree64;

} // namespace geom

#endif // ANTKEEPER_GEOM_QUADTREE_HPP

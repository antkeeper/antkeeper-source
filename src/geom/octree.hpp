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

#ifndef ANTKEEPER_GEOM_OCTREE_HPP
#define ANTKEEPER_GEOM_OCTREE_HPP

#include "geom/hyperoctree.hpp"

namespace geom {

/// An octree, or 3-dimensional hyperoctree.
template <std::size_t D, class T>
using octree = hyperoctree<3, D, T>;

/// Octree with an 8-bit node type (2 depth levels).
typedef octree<1, std::uint8_t> octree8;

/// Octree with a 16-bit node type (4 depth levels).
typedef octree<3, std::uint16_t> octree16;

/// Octree with a 32-bit node type (9 depth levels).
typedef octree<8, std::uint32_t> octree32;

/// Octree with a 64-bit node type (19 depth levels).
typedef octree<18, std::uint64_t> octree64;

} // namespace geom

#endif // ANTKEEPER_GEOM_OCTREE_HPP

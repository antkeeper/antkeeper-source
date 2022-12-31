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

#ifndef ANTKEEPER_GEOM_OCTREE_HPP
#define ANTKEEPER_GEOM_OCTREE_HPP

#include "geom/hyperoctree.hpp"
#include <cstdint>

namespace geom {

/// An octree, or 3-dimensional hyperoctree.
template <std::unsigned_integral T, hyperoctree_order Order>
using octree = hyperoctree<T, 3, Order>;

/// Octree with an 8-bit node type (2 depth levels).
template <hyperoctree_order Order>
using octree8 = octree<std::uint8_t, Order>;

/// Octree with a 16-bit node type (4 depth levels).
template <hyperoctree_order Order>
using octree16 = octree<std::uint16_t, Order>;

/// Octree with a 32-bit node type (9 depth levels).
template <hyperoctree_order Order>
using octree32 = octree<std::uint32_t, Order>;

/// Octree with a 64-bit node type (19 depth levels).
template <hyperoctree_order Order>
using octree64 = octree<std::uint64_t, Order>;

/// Octree with unordered node storage and traversal.
template <std::unsigned_integral T>
using unordered_octree = octree<T, hyperoctree_order::unordered>;

/// Unordered octree with an 8-bit node type (2 depth levels).
typedef unordered_octree<std::uint8_t> unordered_octree8;

/// Unordered octree with a 16-bit node type (4 depth levels).
typedef unordered_octree<std::uint16_t> unordered_octree16;

/// Unordered octree with a 32-bit node type (9 depth levels).
typedef unordered_octree<std::uint32_t> unordered_octree32;

/// Unordered octree with a 64-bit node type (19 depth levels).
typedef unordered_octree<std::uint64_t>unordered_octree64;

} // namespace geom

#endif // ANTKEEPER_GEOM_OCTREE_HPP

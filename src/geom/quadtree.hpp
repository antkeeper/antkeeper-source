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

#ifndef ANTKEEPER_GEOM_QUADTREE_HPP
#define ANTKEEPER_GEOM_QUADTREE_HPP

#include "geom/hyperoctree.hpp"
#include <cstdint>

namespace geom {

/// A quadtree, or 2-dimensional hyperoctree.
template <std::unsigned_integral T, hyperoctree_order Order>
using quadtree = hyperoctree<T, 2, Order>;

/// Quadtree with an 8-bit node type (2 depth levels).
template <hyperoctree_order Order>
using quadtree8 = quadtree<std::uint8_t, Order>;

/// Quadtree with a 16-bit node type (6 depth levels).
template <hyperoctree_order Order>
using quadtree16 = quadtree<std::uint16_t, Order>;

/// Quadtree with a 32-bit node type (13 depth levels).
template <hyperoctree_order Order>
using quadtree32 = quadtree<std::uint32_t, Order>;

/// Quadtree with a 64-bit node type (29 depth levels).
template <hyperoctree_order Order>
using quadtree64 = quadtree<std::uint64_t, Order>;

/// Quadtree with unordered node storage and traversal.
template <std::unsigned_integral T>
using unordered_quadtree = quadtree<T, hyperoctree_order::unordered>;

/// Unordered quadtree with an 8-bit node type (2 depth levels).
typedef unordered_quadtree<std::uint8_t> unordered_quadtree8;

/// Unordered quadtree with a 16-bit node type (6 depth levels).
typedef unordered_quadtree<std::uint16_t> unordered_quadtree16;

/// Unordered quadtree with a 32-bit node type (13 depth levels).
typedef unordered_quadtree<std::uint32_t> unordered_quadtree32;

/// Unordered quadtree with a 64-bit node type (29 depth levels).
typedef unordered_quadtree<std::uint64_t>unordered_quadtree64;

} // namespace geom

#endif // ANTKEEPER_GEOM_QUADTREE_HPP

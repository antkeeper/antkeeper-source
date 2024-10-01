// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_QUADTREE_HPP
#define ANTKEEPER_GEOM_QUADTREE_HPP

#include <engine/geom/hyperoctree.hpp>
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

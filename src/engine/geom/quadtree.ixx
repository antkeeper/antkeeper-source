// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.quadtree;
export import engine.geom.hyperoctree;
import engine.utility.sized_types;

export namespace engine::geom
{
	/// A quadtree, or 2-dimensional hyperoctree.
	template <std::unsigned_integral T, hyperoctree_order Order>
	using quadtree = hyperoctree<T, 2, Order>;

	/// Quadtree with an 8-bit node type (2 depth levels).
	template <hyperoctree_order Order>
	using quadtree8 = quadtree<u8, Order>;

	/// Quadtree with a 16-bit node type (6 depth levels).
	template <hyperoctree_order Order>
	using quadtree16 = quadtree<u16, Order>;

	/// Quadtree with a 32-bit node type (13 depth levels).
	template <hyperoctree_order Order>
	using quadtree32 = quadtree<u32, Order>;

	/// Quadtree with a 64-bit node type (29 depth levels).
	template <hyperoctree_order Order>
	using quadtree64 = quadtree<u64, Order>;

	/// Quadtree with unordered node storage and traversal.
	template <std::unsigned_integral T>
	using unordered_quadtree = quadtree<T, hyperoctree_order::unordered>;

	/// Unordered quadtree with an 8-bit node type (2 depth levels).
	using unordered_quadtree8 = unordered_quadtree<u8>;

	/// Unordered quadtree with a 16-bit node type (6 depth levels).
	using unordered_quadtree16 = unordered_quadtree<u16>;

	/// Unordered quadtree with a 32-bit node type (13 depth levels).
	using unordered_quadtree32 = unordered_quadtree<u32>;

	/// Unordered quadtree with a 64-bit node type (29 depth levels).
	using unordered_quadtree64 = unordered_quadtree<u64>;
}

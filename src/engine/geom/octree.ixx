// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.octree;
export import engine.geom.hyperoctree;
import engine.utility.sized_types;

export namespace engine::geom
{
	/// An octree, or 3-dimensional hyperoctree.
	template <std::unsigned_integral T, hyperoctree_order Order>
	using octree = hyperoctree<T, 3, Order>;

	/// Octree with an 8-bit node type (2 depth levels).
	template <hyperoctree_order Order>
	using octree8 = octree<u8, Order>;

	/// Octree with a 16-bit node type (4 depth levels).
	template <hyperoctree_order Order>
	using octree16 = octree<u16, Order>;

	/// Octree with a 32-bit node type (9 depth levels).
	template <hyperoctree_order Order>
	using octree32 = octree<u32, Order>;

	/// Octree with a 64-bit node type (19 depth levels).
	template <hyperoctree_order Order>
	using octree64 = octree<u64, Order>;

	/// Octree with unordered node storage and traversal.
	template <std::unsigned_integral T>
	using unordered_octree = octree<T, hyperoctree_order::unordered>;

	/// Unordered octree with an 8-bit node type (2 depth levels).
	using unordered_octree8 = unordered_octree<u8>;

	/// Unordered octree with a 16-bit node type (4 depth levels).
	using unordered_octree16 = unordered_octree<u16>;

	/// Unordered octree with a 32-bit node type (9 depth levels).
	using unordered_octree32 = unordered_octree<u32>;

	/// Unordered octree with a 64-bit node type (19 depth levels).
	using unordered_octree64 = unordered_octree<u64>;
}

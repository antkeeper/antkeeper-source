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

#ifndef ANTKEEPER_GEOM_HYPEROCTREE_HPP
#define ANTKEEPER_GEOM_HYPEROCTREE_HPP

#include "math/compile.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <set>
#include <type_traits>
#include <unordered_set>

namespace geom {

/// Orders in which hyperoctree nodes can be stored and traversed.
enum class hyperoctree_order
{
	/// Hyperoctree nodes are unordered, potentially resulting in faster insertions through the internal use of `std::unordered_set` rather than `std::set`.
	unordered,
	
	/// Hyperoctree nodes are stored and traversed in depth-first preorder.
	dfs_pre,
	
	/// Hyperoctree nodes are stored and traversed in breadth-first order.
	bfs
};

/// @private
template <std::unsigned_integral T>
using hyperoctree_dfs_pre_compare = std::less<T>;

/// @private
template <std::unsigned_integral T, std::size_t DepthBits>
struct hyperoctree_bfs_compare
{
	constexpr bool operator()(const T& lhs, const T& rhs) const noexcept
	{
		return std::rotr(lhs, DepthBits) < std::rotr(rhs, DepthBits);
	}
};

/// @private
template <hyperoctree_order Order, std::unsigned_integral T, std::size_t DepthBits>
struct hyperoctree_container {};

/// @private
template <std::unsigned_integral T, std::size_t DepthBits>
struct hyperoctree_container<hyperoctree_order::unordered, T, DepthBits>
{
	typedef std::unordered_set<T> type;
};

/// @private
template <std::unsigned_integral T, std::size_t DepthBits>
struct hyperoctree_container<hyperoctree_order::dfs_pre, T, DepthBits>
{
	typedef std::set<T, hyperoctree_dfs_pre_compare<T>> type;
};

/// @private
template <std::unsigned_integral T, std::size_t DepthBits>
struct hyperoctree_container<hyperoctree_order::bfs, T, DepthBits>
{
	typedef std::set<T, hyperoctree_bfs_compare<T, DepthBits>> type;
};

/**
 * Hashed linear hyperoctree.
 *
 * @tparam T Unsigned integral node identifier type.
 * @tparam N Number of dimensions.
 * @tparam Order Order in which nodes are stored and traversed.
 *
 * @see http://codervil.blogspot.com/2015/10/octree-node-identifiers.html
 * @see https://geidav.wordpress.com/2014/08/18/advanced-octrees-2-node-representations/
 */
template <std::unsigned_integral T, std::size_t N, hyperoctree_order Order = hyperoctree_order::dfs_pre>
class hyperoctree
{
private:
	/**
	 * Finds the maximum hyperoctree depth level from the size of the node type `T` and number of dimensions `N`.
	 *
	 * @return Maximum hyperoctree depth level.
	 *
	 * @note There is likely a more elegant formula for this. Information about the 2D and 3D cases is given below:
	 *
	 * 2D:
	 *   8 bit ( 1 byte) = max depth   1 (  4 loc bits, 1 depth bits, 1 divider bit) =   6 bits
	 *  16 bit ( 2 byte) = max depth   5 ( 12 loc bits, 3 depth bits, 1 divider bit) =  16 bits
	 *  32 bit ( 4 byte) = max depth  12 ( 26 loc bits, 4 depth bits, 1 divider bit) =  31 bits
	 *  64 bit ( 8 byte) = max depth  28 ( 58 loc bits, 5 depth bits, 1 divider bit) =  64 bits
	 * 128 bit (16 byte) = max depth  59 (120 loc bits, 6 depth bits, 1 divider bit) = 127 bits
	 * 256 bit (32 byte) = max depth 123 (248 loc bits, 7 depth bits, 1 divider bit) = 256 bits
	 *
	 * @see https://oeis.org/A173009
	 * 
	 * 3D:
	 *   8 bit ( 1 byte) = max depth  1 (  6 loc bits, 1 depth bits, 1 divider bit) =   8 bits
	 *  16 bit ( 2 byte) = max depth  3 ( 12 loc bits, 2 depth bits, 1 divider bit) =  15 bits
	 *  32 bit ( 4 byte) = max depth  8 ( 27 loc bits, 4 depth bits, 1 divider bit) =  32 bits
	 *  64 bit ( 8 byte) = max depth 18 ( 57 loc bits, 5 depth bits, 1 divider bit) =  63 bits
	 * 128 bit (16 byte) = max depth 39 (120 loc bits, 6 depth bits, 1 divider bit) = 127 bits
	 * 256 bit (32 byte) = max depth 81 (243 loc bits, 7 depth bits, 1 divider bit) = 251 bits
	 *
	 * @see https://oeis.org/A178420
	 */
	static consteval std::size_t find_max_depth() noexcept
	{
		std::size_t max_depth = 0;
		for (std::size_t i = 1; i <= sizeof(T) * 8; ++i)
		{
			const std::size_t location_bits = sizeof(T) * 8 - i;
			max_depth = location_bits / N - 1;
			const std::size_t depth_bits = static_cast<std::size_t>(std::bit_width(max_depth));
			
			if (depth_bits + location_bits < sizeof(T) * 8)
				break;
		}
		return static_cast<T>(max_depth);
	}
	
public:
	/// Node identifier type.
	typedef T node_type;
	
	/// Number of dimensions.
	static constexpr std::size_t dimensions = N;
	
	/// Node storage and traversal order.
	static constexpr hyperoctree_order order = Order;
	
	/// Maximum node depth level.
	static constexpr node_type max_depth = find_max_depth();
	
	/// Number of bits in the node type.
	static constexpr node_type node_bits = sizeof(node_type) * 8;
	
	/// Number of bits required to encode the depth of a node.
	static constexpr node_type depth_bits = std::bit_width(max_depth);
	
	/// Number of bits required to encode the Morton location code of a node.
	static constexpr node_type location_bits = (max_depth + 1) * N;
	
	/// Number of bits separating the depth and Morton location code in a node identifier.
	static constexpr node_type divider_bits = node_bits - (depth_bits + location_bits);
	
	/// Number of children per node.
	static constexpr node_type children_per_node = math::compile::exp2<node_type>(N);
	
	/// Number of siblings per node.
	static constexpr node_type siblings_per_node = children_per_node - 1;
	
	/// Resolution in each dimension.
	static constexpr node_type resolution = math::compile::exp2<node_type>(max_depth);
	
	/// Number of nodes in a full hyperoctree.
	static constexpr std::size_t max_node_count = (math::compile::pow<std::size_t>(resolution * 2, N) - 1) / siblings_per_node;
	
	/// Node identifier of the persistent root node.
	static constexpr node_type root = 0;
	
	/// Node container type.
	typedef typename hyperoctree_container<order, node_type, depth_bits>::type container_type;
	
	/// Iterator type.
	typedef typename container_type::iterator iterator;
	
	/// Constant iterator type.
	typedef typename container_type::const_iterator const_iterator;
	
	/// Reverse iterator type.
	typedef std::conditional<order != hyperoctree_order::unordered, std::reverse_iterator<iterator>, iterator>::type reverse_iterator;
	
	/// Constant reverse iterator type.
	typedef std::conditional<order != hyperoctree_order::unordered, std::reverse_iterator<const_iterator>, const_iterator>::type const_reverse_iterator;
	
	/// @name Nodes
	/// @{
	/**
	 * Extracts the depth of a node from its identifier.
	 *
	 * @param node Node identifier.
	 *
	 * @return Depth of the node.
	 */
	static inline constexpr node_type depth(node_type node) noexcept
	{
		constexpr node_type mask = math::compile::exp2<node_type>(depth_bits) - 1;
		return node & mask;
	}
	
	/**
	 * Extracts the Morton location code of a node from its identifier.
	 *
	 * @param node Node identifier.
	 *
	 * @return Morton location code of the node.
	 */
	static inline constexpr node_type location(node_type node) noexcept
	{
		return node >> ((node_bits - 1) - depth(node) * N);
	}
	
	/**
	 * Extracts the depth and Morton location code of a node from its identifier.
	 *
	 * @param node Node identifier.
	 *
	 * @return Array containing the depth of the node, followed by the Morton location code of the node.
	 */
	static constexpr std::array<node_type, 2> split(node_type node) noexcept
	{
		const node_type depth = hyperoctree::depth(node);
		const node_type location = node >> ((node_bits - 1) - depth * N);
		return {depth, location};
	}
	
	/**
	 * Constructs an identifier for a node at the given depth and location.
	 *
	 * @param depth Depth level.
	 * @param location Morton location code.
	 *
	 * @return Identifier of a node at the given depth and location.
	 *
	 * @warning If @p depth exceeds `max_depth`, the returned node identifier is not valid.
	 */
	static inline constexpr node_type node(node_type depth, node_type location) noexcept
	{
		return (location << ((node_bits - 1) - depth * N)) | depth;
	}
	
	/**
	 * Constructs an identifier for the ancestor of a node at a given depth.
	 *
	 * @param node Node identifier.
	 * @param depth Absolute depth of an ancestor node.
	 *
	 * @return Identifier of the ancestor of the node at the given depth.
	 *
	 * @warning If @p depth exceeds the depth of @p node, the returned node identifier is not valid.
	 */
	static inline constexpr node_type ancestor(node_type node, node_type depth) noexcept
	{
		const node_type mask = (~node_type(0)) << ((node_bits - 1) - depth * N);
		return (node & mask) | depth;
	}
	
	/**
	 * Constructs an identifier for the parent of a node.
	 *
	 * @param node Node identifier.
	 *
	 * @return Identifier of the parent node.
	 */
	static inline constexpr node_type parent(node_type node) noexcept
	{
		return ancestor(node, depth(node) - 1);
	}
	
	/**
	 * Constructs an identifier for the nth sibling of a node.
	 *
	 * @param node Node identifier.
	 * @param n Offset to a sibling, automatically wrapped to `[0, siblings_per_node]`.
	 *
	 * @return Identifier of the nth sibling node.
	 */
	static constexpr node_type sibling(node_type node, node_type n) noexcept
	{
		constexpr node_type mask = (1 << N) - 1;
		const auto [depth, location] = split(node);
		const node_type sibling_location = (location & (~mask)) | ((location + n) & mask);
		return hyperoctree::node(depth, sibling_location);
	}
	
	/**
	 * Constructs an identifier for the nth child of a node.
	 *
	 * @param node Node identifier.
	 * @param n Offset to a sibling of the first child node, automatically wrapped to `[0, siblings_per_node]`.
	 *
	 * @return Identifier of the nth child node.
	 */
	static inline constexpr node_type child(node_type node, T n) noexcept
	{
		return sibling(node + 1, n);
	}
	
	/**
	 * Constructs an identifier for the first common ancestor of two nodes
	 *
	 * @param a Identifier of the first node.
	 * @param b Identifier of the second node.
	 *
	 * @return Identifier of the first common ancestor of the two nodes.
	 */
	static constexpr node_type common_ancestor(node_type a, node_type b) noexcept
	{
		const node_type bits = std::min<node_type>(depth(a), depth(b)) * N;
		const node_type marker = (node_type(1) << (node_bits - 1)) >> bits;
		const node_type depth = node_type(std::countl_zero((a ^ b) | marker) / N);
		return ancestor(a, depth);
	}
	/// @}
	
	/// Constructs a hyperoctree with a single root node.
	hyperoctree():
		nodes({root})
	{}
	
	/// @name Iterators
	/// @{
	/**
	 * Returns an iterator to the first node, in the traversal order specified by hyperoctree::order.
	 *
	 * @note Node identifiers cannot be modified through iterators.
	 */
	/// @{
	inline iterator begin() noexcept
	{
		return nodes.begin();
	}
	inline const_iterator begin() const noexcept
	{
		return nodes.begin();
	}
	inline const_iterator cbegin() const noexcept
	{
		return nodes.cbegin();
	}
	/// @}
	
	/**
	 * Returns an iterator to the node following the last node, in the traversal order specified by hyperoctree::order.
	 *
	 * @note Node identifiers cannot be modified through iterators.
	 */
	/// @{
	inline iterator end() noexcept
	{
		return nodes.end();
	}
	inline const_iterator end() const noexcept
	{
		return nodes.end();
	}
	inline const_iterator cend() const noexcept
	{
		return nodes.cend();
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the first node of the revered hyperoctree, in the traversal order specified by hyperoctree::order.
	 *
	 * @note Node identifiers cannot be modified through iterators.
	 * @note If the hyperoctree is unordered, reverse iteration and forward iteration will be identical.
	 */
	/// @{
	inline reverse_iterator rbegin() noexcept
	{
		if constexpr (order != hyperoctree_order::unordered)
			return nodes.rbegin();
		else
			return nodes.begin();
	}
	inline const_reverse_iterator rbegin() const noexcept
	{
		if constexpr (order != hyperoctree_order::unordered)
			return nodes.rbegin();
		else
			return nodes.begin();
	}
	inline const_reverse_iterator crbegin() const noexcept
	{
		if constexpr (order != hyperoctree_order::unordered)
			return nodes.crbegin();
		else
			return nodes.cbegin();
	}
	/// @}
	
	/**
	 * Returns a reverse iterator to the node following the last node of the reverse hyperoctree, in the traversal order specified by hyperoctree::order.
	 *
	 * @note Node identifiers cannot be modified through iterators.
	 * @note If the hyperoctree is unordered, reverse iteration and forward iteration will be identical.
	 */
	/// @{
	inline reverse_iterator rend() noexcept
	{
		if constexpr (order != hyperoctree_order::unordered)
			return nodes.rend();
		else
			return nodes.end();
	}
	inline const_reverse_iterator rend() const noexcept
	{
		if constexpr (order != hyperoctree_order::unordered)
			return nodes.rend();
		else
			return nodes.end();
	}
	inline const_reverse_iterator crend() const noexcept
	{
		if constexpr (order != hyperoctree_order::unordered)
			return nodes.crend();
		else
			return nodes.cend();
	}
	/// @}
	/// @}
	
	/// @name Capacity
	/// @{
	/**
	 * Checks if the hyperoctree has no nodes.
	 *
	 * @return `true` if the hyperoctree is empty, `false` otherwise.
	 *
	 * @note This function should always return `false`, as the root node is persistent.
	 */
	inline bool empty() const noexcept
	{
		return nodes.empty();
	}
	
	/**
	 * Checks if the hyperoctree is full.
	 *
	 * @return `true` if the hyperoctree is full, `false` otherwise.
	 */
	inline bool full() const noexcept
	{
		return size() == max_size();
	}
	
	/**
	 * Returns the number of nodes in the hyperoctree.
	 *
	 * @return Number of nodes in the hyperoctree.
	 *
	 * @note Hyperoctree size will always be greater than or equal to one, as the root node is persistent.
	 */
	inline std::size_t size() const noexcept
	{
		return nodes.size();
	}
	
	/// Returns the total number of nodes the hyperoctree is capable of containing.
	constexpr std::size_t max_size() const noexcept
	{
		return max_node_count;
	}
	/// @}
	
	/// @name Modifiers
	/// @{
	/**
	 * Erases all nodes except the root node, which is persistent.
	 */
	inline void clear()
	{
		nodes = {root};
	}
	
	/**
	 * Inserts a node and its siblings into the hyperoctree, inserting ancestors as necessary.
	 *
	 * @param node Node to insert.
	 *
	 * @note The root node is persistent and does not need to be inserted.
	 */
	void insert(node_type node)
	{
		if (contains(node))
			return;
		
		// Insert node
		nodes.emplace(node);
		
		// Insert node siblings
		for (node_type i = 1; i < children_per_node; ++i)
			nodes.emplace(sibling(node, i));
		
		// Insert node ancestors
		insert(parent(node));
	}
	
	/**
	 * Erases a node, along with its descendants, siblings, and descendants of siblings.
	 *
	 * @param node Identifier of the node to erase.
	 *
	 * @note The root node is persistent and cannot be erased.
	 */
	void erase(node_type node)
	{
		if (node == root || !contains(node))
			return;
		
		// Erase node and its descendants
		nodes.erase(node);
		erase(child(node, 0));
		
		// Erase node siblings
		for (node_type i = 0; i < siblings_per_node; ++i)
		{
			node = sibling(node, 1);
			
			// Erase sibling and its descendants
			nodes.erase(node);
			erase(child(node, 0));
		}
	}
	/// @}
	
	/// @name Lookup
	/// @{
	/**
	 * Checks if a node is contained within the hyperoctree.
	 *
	 * @param node Identifier of the node to check for.
	 *
	 * @return `true` if the hyperoctree contains the node, `false` otherwise.
	 */
	inline bool contains(node_type node) const
	{
		return nodes.contains(node);
	}
	
	/**
	 * Checks if a node has no children.
	 *
	 * @param node Node identififer.
	 *
	 * @return `true` if the node has no children, and `false` otherwise.
	 */
	inline bool is_leaf(node_type node) const
	{
		return !contains(child(node, 0));
	}
	/// @}
	
private:
	container_type nodes;
};

/// Hyperoctree with unordered node storage and traversal.
template <std::unsigned_integral T, std::size_t N>
using unordered_hyperoctree = hyperoctree<T, N, hyperoctree_order::unordered>;

} // namespace geom

#endif // ANTKEEPER_GEOM_HYPEROCTREE_HPP

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

#ifndef ANTKEEPER_OCTREE_HPP
#define ANTKEEPER_OCTREE_HPP

#include <cstdint>
#include <limits>
#include <type_traits>
#include <unordered_set>
#include <stack>

/**
 * A general purpose (hashed) linear octree. Nodes are integer identifiers and no other data is stored in the octree.
 *
 * @tparam T Integer node type. Must be 16-bit, 32-bit, or 64-bit.
 *
 * @see http://codervil.blogspot.com/2015/10/octree-node-identifiers.html
 * @see https://geidav.wordpress.com/2014/08/18/advanced-octrees-2-node-representations/
 */
template <class T>
class octree
{
private:
	/// Compile-time calculation of the minimum bits required to represent `n` state changes.
	static constexpr T ceil_log2(T n);

public:
	static_assert(std::is_integral<T>::value, "Node type must be integral.");
	static_assert(sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8, "Node type must be 16-bit, 32-bit, or 64-bit.");

	/// Maximum octree depth
	static constexpr T max_depth = (sizeof(T) == 2) ? 3 : (sizeof(T) == 4) ? 8 : 18;

	/// Number of bits in the node type
	static constexpr T node_bits = sizeof(T) * 8;

	/// Number of bits used to encode the depth of a node.
	static constexpr T depth_bits = ceil_log2(max_depth + 1);

	/// Number of bits used to encode the Morton code location a node.
	static constexpr T location_bits = (max_depth + 1) * 3;

	/// Integer node type.
	typedef T node_type;

	/// Root node which is always guaranteed to exist.
	static constexpr node_type root = 0;

	/**
	 * Accesses nodes in their internal hashmap order.
	 */
	struct unordered_iterator
	{
		inline unordered_iterator(const unordered_iterator& other): set_iterator(other.set_iterator) {};
		inline unordered_iterator& operator=(const unordered_iterator& other) { this->set_iterator = other.set_iterator; return *this; };
		inline unordered_iterator& operator++() { ++(this->set_iterator); return *this; };
		inline unordered_iterator& operator--() { --(this->set_iterator); return *this; };
		inline bool operator==(const unordered_iterator& other) const { return this->set_iterator == other.set_iterator; };
		inline bool operator!=(const unordered_iterator& other) const { return this->set_iterator != other.set_iterator; };
		inline const node_type& operator*() const { return *this->set_iterator; };
	private:
		friend class octree;
		inline explicit unordered_iterator(const typename std::unordered_set<node_type>::const_iterator& it): set_iterator(it) {};
		typename std::unordered_set<node_type>::const_iterator set_iterator;
	};

	/**
	 * Accesses nodes in z-order. TODO: I think this can be done without a stack.
	 */
	struct iterator
	{
		inline iterator(const iterator& other): octree(other.octree), stack(other.stack) {};
		inline iterator& operator=(const iterator& other) { this->octree = other.octree; this->stack = other.stack; return *this; };
		iterator& operator++();
		inline bool operator==(const iterator& other) const { return **this == *other; };
		inline bool operator!=(const iterator& other) const { return **this != *other; };
		inline const node_type& operator*() const { return stack.top(); };
	private:
		friend class octree;
		inline explicit iterator(const octree* octree, node_type node): octree(octree), stack({node}) {};
		const octree* octree;
		std::stack<node_type> stack;
	};

	/**
	 * Returns the depth of a node.
	 *
	 * @param node Node.
	 * @return Depth of the node.
	 */
	static T depth(node_type node);

	/**
	 * Returns the Morton code location of a node.
	 *
	 * @param node Node.
	 * @return Morton code location of the node.
	 */
	static T location(node_type node);

	/**
	 * Returns the node at the given depth and location.
	 *
	 * @param depth Node depth.
	 * @param location Node Morton code location.
	 */
	static node_type node(T depth, T location);

	/**
	 * Returns the ancestor of a node at the specified depth.
	 *
	 * @param node Node whose ancestor will be located.
	 * @param depth Absolute depth of the ancestors.
	 * @return Ancestral node.
	 */
	static node_type ancestor(node_type node, T depth);

	/**
	 * Returns the parent of a node.
	 *
	 * @param node Node.
	 * @return Parent node.
	 */
	static node_type parent(node_type node);

	/**
	 * Returns the nth sibling of a node.
	 *
	 * @param node Node.
	 * @param n Offset to next sibling. (Automatically wraps to 0..7)
	 * @return Next sibling node.
	 */
	static node_type sibling(node_type node, T n);

	/**
	 * Returns the nth child of a node.
	 *
	 * @param node Parent node.
	 * @param n Offset to the nth sibling of the first child node. (Automatically wraps to 0..7)
	 * @return nth child node.
	 */
	static node_type child(node_type node, T n);

	/**
	 * Calculates the first common ancestor of two nodes.
	 *
	 * @param a First node.
	 * @param b Second node.
	 * @return First common ancestor of the two nodes.
	 */
	static node_type common_ancestor(node_type a, node_type b);

	/// Creates an octree with a single root node.
	octree();

	/// Returns a z-order iterator to the root node.
	iterator begin() const;

	/// Returns a z-order iterator indicating the end of a traversal.
	iterator end() const;

	/// Returns an iterator to the specified node.
	iterator find(node_type node) const;

	/// Returns an unordered iterator indicating the beginning of a traversal.
	unordered_iterator unordered_begin() const;

	/// Returns an unordered iterator indicating the end of a traversal.
	unordered_iterator unordered_end() const;

	/**
	 * Inserts a node and its siblings into the octree, creating its ancestors as necessary. Note: The root node is persistent and cannot be inserted.
	 *
	 * @param node Node to insert.
	 */
	void insert(node_type node);

	/**
	 * Erases a node along with its siblings and descendants. Note: The root node is persistent and cannot be erased.
	 *
	 * @param node Node to erase.
	 */
	void erase(node_type node);

	/**
	 * Erases all nodes except the root.
	 */
	void clear();

	/// Returns `true` if the node exists in the octree, and `false` otherwise.
	bool exists(node_type node) const;

	/// Returns `true` if the node has no children, and `false` otherwise.
	bool is_leaf(node_type node) const;

	/// Returns the number of nodes in the octree.
	std::size_t size() const;

private:
	/// Compile-time pow()
	static constexpr T pow(T x, T exponent);

	/// Count leading zeros
	static T clz(T x);

	std::unordered_set<node_type> nodes;
};

/**
 * Octree with a 16-bit node type and a maximum depth of `3`.
 */
typedef octree<std::uint16_t> octree16;

/**
 * Octree with a 32-bit node type and a maximum depth of `8`.
 */
typedef octree<std::uint32_t> octree32;

/**
 * Octree with a 64-bit node type and a maximum depth of `18`.
 */
typedef octree<std::uint64_t> octree64;

template <typename T>
typename octree<T>::iterator& octree<T>::iterator::operator++()
{
	// Get next node from top of stack
	node_type node = stack.top();
	stack.pop();

	// If the node has children
	if (!octree->is_leaf(node))
	{
		// Push first child onto the stack
		for (T i = 0; i < 8; ++i)
			stack.push(child(node, 7 - i));
	}

	if (stack.empty())
		stack.push(std::numeric_limits<T>::max());

	return *this;
}

template <class T>
constexpr T octree<T>::ceil_log2(T n)
{
	return (n <= 1) ? 0 : ceil_log2((n + 1) / 2) + 1;
}

template <class T>
inline T octree<T>::depth(node_type node)
{
	// Extract depth using a bit mask
	constexpr T mask = pow(2, depth_bits) - 1;
	return node & mask;
}

template <class T>
inline T octree<T>::location(node_type node)
{
	return node >> ((node_bits - 1) - depth(node) * 3);
}

template <class T>
inline typename octree<T>::node_type octree<T>::node(T depth, T location)
{
	return (location << ((node_bits - 1) - depth * 3)) | depth;
}

template <class T>
inline typename octree<T>::node_type octree<T>::ancestor(node_type node, T depth)
{
	const T mask = std::numeric_limits<T>::max() << ((node_bits - 1) - depth * 3);
    return (node & mask) | depth;
}

template <class T>
inline typename octree<T>::node_type octree<T>::parent(node_type node)
{
	return ancestor(node, depth(node) - 1);
}

template <class T>
inline typename octree<T>::node_type octree<T>::sibling(node_type node, T n)
{
	T depth = octree::depth(node);
	T location = node >> ((node_bits - 1) - depth * 3);
	return octree::node(depth, (location & (~0b111)) | ((location + n) & 0b111));
}

template <class T>
inline typename octree<T>::node_type octree<T>::child(node_type node, T n)
{
	return sibling(node + 1, n);
}

template <class T>
inline typename octree<T>::node_type octree<T>::common_ancestor(node_type a, node_type b)
{
	T bits = std::min<T>(depth(a), depth(b)) * 3;
	T marker = (T(1) << (node_bits - 1)) >> bits;
	T depth = clz((a ^ b) | marker) / 3;
	return ancestor(a, depth);
}

template <class T>
inline octree<T>::octree():
	nodes({0})
{}

template <class T>
void octree<T>::insert(node_type node)
{
	if (exists(node))
		return;
	
	// Insert node
	nodes.emplace(node);

	// Insert siblings
	for (T i = 1; i < 8; ++i)
		nodes.emplace(sibling(node, i));
	
	// Insert parent as necessary
	node_type parent = octree::parent(node);
	if (!exists(parent))
		insert(parent);
}

template <class T>
void octree<T>::erase(node_type node)
{
	// Don't erase the root!
	if (node == root)
		return;
	
	for (T i = 0; i < 8; ++i)
	{
		// Erase node
		nodes.erase(node);

		// Erase descendants
		if (!is_leaf(node))
		{
			for (T j = 0; j < 8; ++j)
				erase(child(node, j));
		}

		// Go to next sibling
		if (i < 7)
			node = sibling(node, i);
	}
}

template <class T>
void octree<T>::clear()
{
	nodes = {0};
}

template <class T>
inline bool octree<T>::exists(node_type node) const
{
	return (nodes.find(node) != nodes.end());
}

template <class T>
inline bool octree<T>::is_leaf(node_type node) const
{
	return !exists(child(node, 0));
}

template <class T>
inline std::size_t octree<T>::size() const
{
	return nodes.size();
}

template <class T>
typename octree<T>::iterator octree<T>::begin() const
{
	return iterator(this, octree::root);
}

template <class T>
typename octree<T>::iterator octree<T>::end() const
{
	return iterator(this, std::numeric_limits<T>::max());
}

template <class T>
typename octree<T>::iterator octree<T>::find(node_type node) const
{
	return exists(node) ? iterator(node) : end();
}

template <class T>
typename octree<T>::unordered_iterator octree<T>::unordered_begin() const
{
	return unordered_iterator(nodes.begin());
}

template <class T>
typename octree<T>::unordered_iterator octree<T>::unordered_end() const
{
	return unordered_iterator(nodes.end());
}

template <class T>
constexpr T octree<T>::pow(T x, T exponent)
{
	return (exponent == 0) ? 1 : x * pow(x, exponent - 1);
}

template <class T>
T octree<T>::clz(T x)
{
	if (!x)
		return sizeof(T) * 8;
	
	#if defined(__GNU__)
		return __builtin_clz(x);
	#else
		T n = 0;

		while ((x & (T(1) << (8 * sizeof(x) - 1))) == 0)
		{
			x <<= 1;
			++n;
		}

		return n;
	#endif
}

#endif // ANTKEEPER_OCTREE_HPP


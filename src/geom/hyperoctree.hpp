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

#ifndef ANTKEEPER_GEOM_HYPEROCTREE_HPP
#define ANTKEEPER_GEOM_HYPEROCTREE_HPP

#include <cstdint>
#include <limits>
#include <type_traits>
#include <unordered_set>
#include <stack>

namespace geom {

/**
 * Hashed linear hyperoctree.
 *
 * @see http://codervil.blogspot.com/2015/10/octree-node-identifiers.html
 * @see https://geidav.wordpress.com/2014/08/18/advanced-octrees-2-node-representations/
 *
 * @tparam N Number of dimensions.
 * @tparam D Max depth.
 *
 * Max depth can likely be determined by a generalized formula. 2D and 3D cases are given below:
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
 *
 * @tparam T Integer node type.
 */
template <std::size_t N, std::size_t D, class T>
class hyperoctree
{
private:
	/// Compile-time calculation of the minimum bits required to represent `n` state changes.
	static constexpr T ceil_log2(T n);

public:
	/// Integral node type.
	typedef T node_type;
	
	/// Ensure the node type is integral
	static_assert(std::is_integral<T>::value, "Node type must be integral.");
	
	/// Maximum node depth.
	static constexpr std::size_t max_depth = D;
	
	/// Number of bits required to encode the depth of a node.
	static constexpr T depth_bits = ceil_log2(max_depth + 1);

	/// Number of bits required to encode the location of a node.
	static constexpr T location_bits = (max_depth + 1) * N;
	
	/// Number of bits in the node type.
	static constexpr T node_bits = sizeof(node_type) * 8;
	
	// Ensure the node type has enough bits
	static_assert(depth_bits + location_bits + 1 <= node_bits, "Size of hyperoctree node type is insufficient to encode the maximum depth");

	/// Number of children per node.
	static constexpr T children_per_node = (N) ? (2 << (N - 1)) : 1;
	
	/// Number of siblings per node.
	static constexpr T siblings_per_node = children_per_node - 1;
	
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
		inline node_type operator*() const { return *this->set_iterator; };
	private:
		friend class hyperoctree;
		inline explicit unordered_iterator(const typename std::unordered_set<node_type>::const_iterator& it): set_iterator(it) {};
		typename std::unordered_set<node_type>::const_iterator set_iterator;
	};

	/**
	 * Accesses nodes in z-order.
	 *
	 * @TODO Can this be implemented without a stack?
	 */
	struct iterator
	{
		inline iterator(const iterator& other): hyperoctree(other.hyperoctree), stack(other.stack) {};
		inline iterator& operator=(const iterator& other) { this->hyperoctree = other.hyperoctree; this->stack = other.stack; return *this; };
		iterator& operator++();
		inline bool operator==(const iterator& other) const { return **this == *other; };
		inline bool operator!=(const iterator& other) const { return **this != *other; };
		inline node_type operator*() const { return stack.top(); };
	private:
		friend class hyperoctree;
		inline explicit iterator(const hyperoctree* hyperoctree, node_type node): hyperoctree(hyperoctree), stack({node}) {};
		const hyperoctree* hyperoctree;
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
	 * @param n Offset to next sibling. (Automatically wraps to `[0, siblings_per_node]`)
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

	/// Creates an hyperoctree with a single root node.
	hyperoctree();

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
	 * Inserts a node and its siblings into the hyperoctree, creating its ancestors as necessary. Note: The root node is persistent and cannot be inserted.
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

	/// Returns `true` if the node is contained within the hyperoctree, and `false` otherwise.
	bool contains(node_type node) const;

	/// Returns `true` if the node has no children, and `false` otherwise.
	bool is_leaf(node_type node) const;

	/// Returns the number of nodes in the hyperoctree.
	std::size_t size() const;

private:
	/// Compile-time pow()
	static constexpr T pow(T x, T exponent);

	/// Count leading zeros
	static T clz(T x);

	std::unordered_set<node_type> nodes;
};

template <std::size_t N, std::size_t D, class T>
typename hyperoctree<N, D, T>::iterator& hyperoctree<N, D, T>::iterator::operator++()
{
	// Get next node from top of stack
	node_type node = stack.top();
	stack.pop();

	// If the node has children
	if (!hyperoctree->is_leaf(node))
	{
		// Push first child onto the stack
		for (T i = 0; i < children_per_node; ++i)
			stack.push(child(node, siblings_per_node - i));
	}

	if (stack.empty())
		stack.push(std::numeric_limits<T>::max());

	return *this;
}

template <std::size_t N, std::size_t D, class T>
constexpr T hyperoctree<N, D, T>::ceil_log2(T n)
{
	return (n <= 1) ? 0 : ceil_log2((n + 1) / 2) + 1;
}

template <std::size_t N, std::size_t D, class T>
inline T hyperoctree<N, D, T>::depth(node_type node)
{
	// Extract depth using a bit mask
	constexpr T mask = pow(2, depth_bits) - 1;
	return node & mask;
}

template <std::size_t N, std::size_t D, class T>
inline T hyperoctree<N, D, T>::location(node_type node)
{
	return node >> ((node_bits - 1) - depth(node) * N);
}

template <std::size_t N, std::size_t D, class T>
inline typename hyperoctree<N, D, T>::node_type hyperoctree<N, D, T>::node(T depth, T location)
{
	return (location << ((node_bits - 1) - depth * N)) | depth;
}

template <std::size_t N, std::size_t D, class T>
inline typename hyperoctree<N, D, T>::node_type hyperoctree<N, D, T>::ancestor(node_type node, T depth)
{
	const T mask = std::numeric_limits<T>::max() << ((node_bits - 1) - depth * N);
    return (node & mask) | depth;
}

template <std::size_t N, std::size_t D, class T>
inline typename hyperoctree<N, D, T>::node_type hyperoctree<N, D, T>::parent(node_type node)
{
	return ancestor(node, depth(node) - 1);
}

template <std::size_t N, std::size_t D, class T>
inline typename hyperoctree<N, D, T>::node_type hyperoctree<N, D, T>::sibling(node_type node, T n)
{
	constexpr T mask = (1 << N) - 1;
	
	T depth = hyperoctree::depth(node);
	T location = node >> ((node_bits - 1) - depth * N);
	
	return hyperoctree::node(depth, (location & (~mask)) | ((location + n) & mask));
}

template <std::size_t N, std::size_t D, class T>
inline typename hyperoctree<N, D, T>::node_type hyperoctree<N, D, T>::child(node_type node, T n)
{
	return sibling(node + 1, n);
}

template <std::size_t N, std::size_t D, class T>
inline typename hyperoctree<N, D, T>::node_type hyperoctree<N, D, T>::common_ancestor(node_type a, node_type b)
{
	T bits = std::min<T>(depth(a), depth(b)) * N;
	T marker = (T(1) << (node_bits - 1)) >> bits;
	T depth = clz((a ^ b) | marker) / N;
	return ancestor(a, depth);
}

template <std::size_t N, std::size_t D, class T>
inline hyperoctree<N, D, T>::hyperoctree():
	nodes({0})
{}

template <std::size_t N, std::size_t D, class T>
void hyperoctree<N, D, T>::insert(node_type node)
{
	if (contains(node))
		return;
	
	// Insert node
	nodes.emplace(node);

	// Insert siblings
	for (T i = 1; i < children_per_node; ++i)
		nodes.emplace(sibling(node, i));
	
	// Insert parent as necessary
	node_type parent = hyperoctree::parent(node);
	if (!contains(parent))
		insert(parent);
}

template <std::size_t N, std::size_t D, class T>
void hyperoctree<N, D, T>::erase(node_type node)
{
	// Don't erase the root!
	if (node == root)
		return;
	
	for (T i = 0; i < children_per_node; ++i)
	{
		// Erase node
		nodes.erase(node);

		// Erase descendants
		if (!is_leaf(node))
		{
			for (T j = 0; j < children_per_node; ++j)
				erase(child(node, j));
		}

		// Go to next sibling
		if (i < siblings_per_node)
			node = sibling(node, i);
	}
}

template <std::size_t N, std::size_t D, class T>
void hyperoctree<N, D, T>::clear()
{
	nodes = {0};
}

template <std::size_t N, std::size_t D, class T>
inline bool hyperoctree<N, D, T>::contains(node_type node) const
{
	return (nodes.find(node) != nodes.end());
}

template <std::size_t N, std::size_t D, class T>
inline bool hyperoctree<N, D, T>::is_leaf(node_type node) const
{
	return !contains(child(node, 0));
}

template <std::size_t N, std::size_t D, class T>
inline std::size_t hyperoctree<N, D, T>::size() const
{
	return nodes.size();
}

template <std::size_t N, std::size_t D, class T>
typename hyperoctree<N, D, T>::iterator hyperoctree<N, D, T>::begin() const
{
	return iterator(this, hyperoctree::root);
}

template <std::size_t N, std::size_t D, class T>
typename hyperoctree<N, D, T>::iterator hyperoctree<N, D, T>::end() const
{
	return iterator(this, std::numeric_limits<T>::max());
}

template <std::size_t N, std::size_t D, class T>
typename hyperoctree<N, D, T>::iterator hyperoctree<N, D, T>::find(node_type node) const
{
	return contains(node) ? iterator(node) : end();
}

template <std::size_t N, std::size_t D, class T>
typename hyperoctree<N, D, T>::unordered_iterator hyperoctree<N, D, T>::unordered_begin() const
{
	return unordered_iterator(nodes.begin());
}

template <std::size_t N, std::size_t D, class T>
typename hyperoctree<N, D, T>::unordered_iterator hyperoctree<N, D, T>::unordered_end() const
{
	return unordered_iterator(nodes.end());
}

template <std::size_t N, std::size_t D, class T>
constexpr T hyperoctree<N, D, T>::pow(T x, T exponent)
{
	return (exponent == 0) ? 1 : x * pow(x, exponent - 1);
}

template <std::size_t N, std::size_t D, class T>
T hyperoctree<N, D, T>::clz(T x)
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

} // namespace geom

#endif // ANTKEEPER_GEOM_HYPEROCTREE_HPP

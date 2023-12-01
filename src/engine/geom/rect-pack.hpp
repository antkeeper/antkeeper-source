// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GEOM_RECT_PACK_HPP
#define ANTKEEPER_GEOM_RECT_PACK_HPP

#include <engine/geom/primitives/rectangle.hpp>
#include <memory>

namespace geom {

/**
 * Node used in 2D rectangle packing.
 *
 * @see geom::rect_pack
 */
template <class T>
struct rect_pack_node
{
	/// Scalar type.
	using scalar_type = T;
	
	/// Rect type.
	using rect_type = rectangle<T>;
	
	/// Pointers to the two children of the node, if any.
	std::unique_ptr<rect_pack_node> children[2];
	
	/// Bounds of the node.
	rect_type bounds{{T{0}, T{0}}, {T{0}, T{0}}};
	
	/// `true` if the node is occupied, `false` otherwise.
	bool occupied{false};
};

/**
 * Packs 2D rectangles.
 *
 * @see geom::rect_pack_node
 *
 * @see http://www.blackpawn.com/texts/lightmaps/
 */
template <class T>
class rect_pack
{
public:
	/// Scalar type.
	using scalar_type = T;
	
	/// Node type.
	using node_type = rect_pack_node<T>;
	
	/**
	 * Creates a rect pack and sets the bounds of the root node.
	 *
	 * @param w Width of the root node.
	 * @param h Height of the root node.
	 */
	rect_pack(scalar_type w, scalar_type h);
	
	/**
	 * Creates an empty rect pack.
	 */
	rect_pack();
	
	/**
	 * Clears the pack and resizes the root node bounds.
	 *
	 * @param w New width of the root node.
	 * @param h New height of the root node.
	 *
	 * @see rect_pack::clear()
	 */
	void resize(scalar_type w, scalar_type h);
	
	/// Clear the pack, deallocating all nodes.
	void clear();
	
	/**
	 * Packs a rect into the rect pack.
	 *
	 * @param w Width of the rect.
	 * @param h Height of the rect.
	 * @return Pointer to the node in which the rect was packed, or `nullptr` if the rect could not be packed.
	 */
	const node_type* pack(scalar_type w, scalar_type h);
	
	/// Returns a reference to the root node.
	const node_type& get_root() const;
	
private:
	static node_type* insert(node_type& parent, scalar_type w, scalar_type h);
	
	static void free();
	
	node_type root;
};

template <class T>
rect_pack<T>::rect_pack(scalar_type w, scalar_type h)
{
	root.bounds = {T{0}, T{0}, w, h};
}

template <class T>
rect_pack<T>::rect_pack():
	rect_pack(0, 0)
{}

template <class T>
void rect_pack<T>::resize(scalar_type w, scalar_type h)
{
	clear();
	root.bounds = {{T{0}, T{0}}, {w, h}};
}

template <class T>
void rect_pack<T>::clear()
{
	root.children[0].reset();
	root.children[1].reset();
	root.occupied = false;
}

template <class T>
const typename rect_pack<T>::node_type* rect_pack<T>::pack(scalar_type w, scalar_type h)
{
	return insert(root, w, h);
}

template <class T>
inline const typename rect_pack<T>::node_type& rect_pack<T>::get_root() const
{
	return root;
}

template <class T>
typename rect_pack<T>::node_type* rect_pack<T>::insert(node_type& node, scalar_type w, scalar_type h)
{
	// If not a leaf node
	if (node.children[0] && node.children[1])
	{
		// Attempt to insert into first child
		node_type* result = insert(*node.children[0], w, h);
		if (result)
		{
			return result;
		}
		
		// Cannot fit in first child, attempt to insert into second child
		return insert(*node.children[1], w, h);
		
	}
	
	// Abort if node occupied
	if (node.occupied)
	{
		return nullptr;
	}
	
	// Determine node dimensions
	scalar_type node_w = node.bounds.max.x() - node.bounds.min.x();
	scalar_type node_h = node.bounds.max.y() - node.bounds.min.y();
	
	// Check if rect is larger than node
	if (w > node_w || h > node_h)
	{
		return nullptr;
	}
	
	// Check for a perfect fit
	if (w == node_w && h == node_h)
	{
		node.occupied = true;
		return &node;
	}
	
	// Split the node
	node.children[0] = std::make_unique<node_type>();
	node.children[1] = std::make_unique<node_type>();
	
	// Determine split direction
	scalar_type dw = node_w - w;
	scalar_type dh = node_h - h;
	if (dw > dh)
	{
		node.children[0]->bounds.min = node.bounds.min;
		node.children[0]->bounds.max = {node.bounds.min.x() + w, node.bounds.max.y()};
		node.children[1]->bounds.min = {node.bounds.min.x() + w, node.bounds.min.y()};
		node.children[1]->bounds.max = {node.bounds.max};
	}
	else
	{
		node.children[0]->bounds.min = node.bounds.min;
		node.children[0]->bounds.max = {node.bounds.max.x(), node.bounds.min.y() + h};
		node.children[1]->bounds.min = {node.bounds.min.x(), node.bounds.min.y() + h};
		node.children[1]->bounds.max = {node.bounds.max};
	}
	
	// Insert into first child
	return insert(*node.children[0], w, h);
}

} // namespace geom

#endif // ANTKEEPER_GEOM_RECT_PACK_HPP

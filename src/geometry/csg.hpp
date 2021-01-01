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

#ifndef ANTKEEPER_CSG_HPP
#define ANTKEEPER_CSG_HPP

#include "utility/fundamental-types.hpp"
#include <list>

namespace csg {

struct plane
{
	float3 normal;
	float distance;
};

struct polygon
{
	std::list<float3> vertices;
	void* shared;
};

/**
 * 3D solid represented by a collection of polygons.
 */
typedef std::list<polygon> solid;

/**
 * BSP tree node.
 */
class bsp_tree
{
public:
	/**
	 * Recursively constructs a BSP tree from a collection of polygons.
	 *
	 * @param polygons Collection of polygons from which to create the BSP tree.
	 */
	explicit bsp_tree(const std::list<polygon>& polygons);

	/**
	 * Destroys a BSP tree.
	 */
	~bsp_tree();

private:
	/// Partition which separates the front and back polygons.
	plane partition;

	/// Set of polygons which are coplanar with the partition.
	std::list<polygon> coplanar_polygons;

	/// Subtree containing all polygons in front of the partition.
	bsp_tree* front;

	/// Subtree containing all polygons behind the partition.
	bsp_tree* back;
};

solid op_union(const solid& a, const solid& b);
solid op_difference(const solid& a, const solid& b);
solid op_intersect(const solid& a, const solid& b);

} // namespace csg

#endif // ANTKEEPER_CSG_HPP


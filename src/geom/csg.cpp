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

#include "csg.hpp"
#include <tuple>

namespace geom {
namespace csg {

enum class polygon_classification
{
	coplanar,
	front,
	back,
	spanning
};

/**
 * Classifies a polygon relative to a partitioning plane.
 *
 * @param partition Partitioning plane relative to which the polygon should be classified.
 * @param poly Polygon to be classified.
 * @return Classification of the polygon, relative to the plane.
 */
static polygon_classification classify_polygon(const plane& partition, const polygon& poly)
{
	for (const float3& vertex: poly.vertices)
	{
		
	}

	return polygon_classification::coplanar;
}

/**
 * Splits a polygon along a partitioning plane.
 *
 * @param poly Polygon to split.
 * @param partition Partitioning plane along which the polygon should be split.
 * @return List of polygons which were formed by splitting the specified polygon along the partitioning plane, along with their respective classifications relative to the partition.
 */
std::list<std::tuple<polygon, polygon_classification>> split_polygon(const polygon& poly, const plane& partition)
{
	return {};
}


bsp_tree::bsp_tree(const std::list<polygon>& polygons):
	front(nullptr),
	back(nullptr)
{
	//partition = polygons.front();

	std::list<polygon> front_polygons;
	std::list<polygon> back_polygons;

	// Classify all polygons relative to this node's partitioning plane
	for (const polygon& p: polygons)
	{
		polygon_classification classification = classify_polygon(partition, p);
		switch (classification)
		{
			case polygon_classification::coplanar:
				coplanar_polygons.push_back(p);
				break;

			case polygon_classification::front:
				front_polygons.push_back(p);
				break;

			case polygon_classification::back:
				back_polygons.push_back(p);
				break;

			case polygon_classification::spanning:
				break;
		}
	}

	if (!front_polygons.empty())
	{
		// Make subtree containing all polygons in front of this node's plane
		front = new bsp_tree(front_polygons);
	}

	if (!back_polygons.empty())
	{
		// Make subtree containing all polygons behind this node's plane
		back = new bsp_tree(back_polygons);
	}
}

bsp_tree::~bsp_tree()
{
	delete front;
	delete back;
}

} // namespace csg
} // namespace geom

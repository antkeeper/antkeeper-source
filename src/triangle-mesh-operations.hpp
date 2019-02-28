/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TRIANGLE_MESH_OPERATIONS_HPP
#define TRIANGLE_MESH_OPERATIONS_HPP

#include <emergent/emergent.hpp>
using namespace Emergent;

/**
 * A single segment in a triangle mesh wrap operation.
 */
struct WrapOperationSegment
{
	/// Pointer to the triangle on which this segment is located
	TriangleMesh::Triangle* triangle;

	/// Pointer to the starting edge of this segment (if any)
	TriangleMesh::Edge* startEdge;

	/// Pointer to the ending edge of this segment (if any)
	TriangleMesh::Edge* endEdge;

	/// Pointer to the starting vertex of this segment (if any)
	TriangleMesh::Vertex* startVertex;

	/// Pointer to the ending vertex of this segment (if any)
	TriangleMesh::Vertex* endVertex;

	/// Barycentric coordinates of the start position of the segment
	Vector3 startPosition; // barycentric

	/// Barycentric coordinates of the end position of the segment
	Vector3 endPosition; // barycentric

	/// Length of the segment
	float length;
};

/**
 * Wraps a vector around a triangle mesh.
 *
 * @param triangle Pointer to the triangle on which the wrap operation will begin.
 * @param position Cartesian coordinates of the starting position.
 * @param direction Initial normalized direction of the wrap operation.
 * @param length Total length of the wrap operation.
 * @param[out] segments List which will be populated with the wrap operation segments
 * @return Total length of the wrap operation. If this quantity is less than the length parameter, this indicates the wrap operation hit a disconnected edge.
 */
float wrap(TriangleMesh::Triangle* triangle, Vector3 position, Vector3 direction, float length, std::vector<WrapOperationSegment>* segments);

#endif // TRIANGLE_MESH_OPERATIONS_HPP


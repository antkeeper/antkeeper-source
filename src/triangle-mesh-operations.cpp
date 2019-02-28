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

#include "triangle-mesh-operations.hpp"

float wrap(TriangleMesh::Triangle* triangle, Vector3 position, Vector3 direction, float length, std::vector<WrapOperationSegment>* segments)
{
	// Get vertex positions and center of starting triangle
	const Vector3* a = &triangle->edge->vertex->position;
	const Vector3* b = &triangle->edge->next->vertex->position;
	const Vector3* c = &triangle->edge->previous->vertex->position;
	Vector3 center = ((*a) + (*b) + (*c)) * (1.0f / 3.0f);
	
	// Project start position and target position onto plane of the starting triangle
	Vector3 coplanarStart = projectOnPlane(position, center, triangle->normal);
	Vector3 coplanarTarget = projectOnPlane(position + direction * length, center, triangle->normal);
	
	// Constrain coplanar position and coplanar target to triangle bounds
	int edgeIndex = -1;
	int vertexIndex = -1;
	Vector3 barycentricStart = projectOnTriangle(coplanarStart, *a, *b, *c, &edgeIndex, &vertexIndex);
	Vector3 offset = cartesian(barycentricStart, *a, *b, *c) - coplanarStart;
	coplanarStart += offset;
	coplanarTarget += offset;
	
	// Calculate coplanar direction
	Vector3 coplanarDirection = glm::normalize(coplanarTarget - coplanarStart);
	
	// Form initial wrap operation segment
	WrapOperationSegment segment;
	segment.triangle = triangle;
	segment.startEdge = nullptr;
	segment.endEdge = nullptr;
	segment.startVertex = nullptr;
	segment.endVertex = nullptr;
	segment.startPosition = barycentricStart;
	segment.endPosition = barycentricStart;
	segment.length = 0.0f;

	// Determine starting edge
	if (edgeIndex == 0)
		segment.startEdge = triangle->edge;
	else if (edgeIndex == 1)
		segment.startEdge = triangle->edge->next;
	else if (edgeIndex == 2)
		segment.startEdge = triangle->edge->previous;

	// Determine starting vertex
	if (vertexIndex == 0)
		segment.startVertex = triangle->edge->vertex;
	else if (vertexIndex == 1)
		segment.startVertex = triangle->edge->next->vertex;
	else if (vertexIndex == 2)
		segment.startVertex = triangle->edge->previous->vertex;

	// Begin wrap operation
	float distance = 0.0f;
	while (1)
	{
		// Calculate coplanar Cartesian start and target positions
		coplanarStart = cartesian(segment.startPosition, *a, *b, *c);
		coplanarTarget = coplanarStart + coplanarDirection * (length - distance);

		// Calculate barycentric end position by projecting coplanar Cartesian target onto triangle
		segment.endPosition = projectOnTriangle(coplanarTarget, *a, *b, *c, &edgeIndex, &vertexIndex);

		// Determine ending edge
		segment.endEdge = nullptr;
		if (edgeIndex == 0)
			segment.endEdge = segment.triangle->edge;
		else if (edgeIndex == 1)
			segment.endEdge = segment.triangle->edge->next;
		else if (edgeIndex == 2)
			segment.endEdge = segment.triangle->edge->previous;

		// Determine ending vertex
		segment.endVertex = nullptr;
		if (vertexIndex == 0)
			segment.endVertex = segment.triangle->edge->vertex;
		else if (vertexIndex == 1)
			segment.endVertex = segment.triangle->edge->next->vertex;
		else if (vertexIndex == 2)
			segment.endVertex = segment.triangle->edge->previous->vertex;

		// Calculate coplanar Cartesian end position
		Vector3 coplanarEnd = cartesian(segment.endPosition, *a, *b, *c);

		// Determine distance traveled
		segment.length = glm::length(coplanarEnd - coplanarStart);
		distance += segment.length;

		// Add segment to wrapped segments
		segments->push_back(segment);

		// Check if wrap has completed
		if ((length - distance ) < 0.0001f || (!segment.endEdge && !segment.endVertex))
		{
			distance = length;
			break;
		}

		// Check if a disconnectd edge was hit
		if (segment.endEdge && !segment.endEdge->symmetric)
		{
			break;
		}

		// Reorientate coplanar direction
		if (segment.triangle->normal != segment.endEdge->symmetric->triangle->normal)
		{
			coplanarDirection = glm::normalize(glm::rotation(segment.triangle->normal, segment.endEdge->symmetric->triangle->normal) * coplanarDirection);
		}

		// Move to the connected triangle
		segment.triangle = segment.endEdge->symmetric->triangle;
		segment.startEdge = segment.endEdge->symmetric;
		segment.endEdge = nullptr;
		segment.startVertex = nullptr;
		segment.endVertex = nullptr;

		// Get vertex positions of triangle
		a = &segment.triangle->edge->vertex->position;
		b = &segment.triangle->edge->next->vertex->position;
		c = &segment.triangle->edge->previous->vertex->position;
		
		// Calculate barycentric starting position of the next segment
		segment.startPosition = normalize_barycentric(barycentric(coplanarEnd, *a, *b, *c));
	}

	return distance;
}


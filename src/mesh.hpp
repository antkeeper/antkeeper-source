/*
 * Copyright (C) 2017  Christopher J. Howard
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

#ifndef MESH_HPP
#define MESH_HPP

#include <emergent/emergent.hpp>
#include <string>
#include <vector>
#include <tuple>

using namespace Emergent;

struct navigator
{
	glm::vec3 position;	// World-space cartesian coordinates
	
	float heading;

	WingedEdge::Triangle* triangle;
	glm::vec3 barycentric; // Current barycentric coordinates
};


bool loadHeightmap(const std::string& filename, glm::vec3 scale, WingedEdge* mesh);

bool loadHeightmapBase(const std::string& filename, glm::vec3 scale, float floor, WingedEdge* mesh);

// Recursive function which moves a navigator along the surface of a navmesh.
void move(const WingedEdge* mesh, WingedEdge::Triangle* triangle, const glm::vec3& start, const glm::vec3& target, std::vector<WingedEdge::Triangle*>* visited, glm::vec3* end);

// Checks for intersection between a ray and a triangle
// The first element in the tuple indicates whether or not an intersection occurred. The second element indicates the distance from the origin to point of intersection. The third element is the barycentric U coordinate. The fourth element is the barycentric V coordinate.
// @see http://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
// @see http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-9-ray-triangle-intersection/m-ller-trumbore-algorithm/
std::tuple<bool, float, float, float> intersects(const glm::vec3& origin, const glm::vec3& direction, const WingedEdge::Triangle& triangle);

// Checks for intersection between a ray and a mesh.
// The first element in the tuple indicates whether or not an intersection occurred. The second and third elements indicate the distance from the origin to the nearest and farthest points of intersection, respectively. The fourth and fifth elements contain the indices of the nearest and farthest intersected triangles, respectively.
std::tuple<bool, float, float, std::size_t, std::size_t> intersects(const glm::vec3& origin, const glm::vec3& direction, const WingedEdge& mesh);

#endif // MESH_HPP


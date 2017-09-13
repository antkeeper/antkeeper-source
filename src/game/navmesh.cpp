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

#include "navmesh.hpp"
#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <limits>

Navmesh::Navmesh()
{}

Navmesh::~Navmesh()
{
	destroy();
}

bool Navmesh::create(const std::vector<Vector3>& vertices, const std::vector<std::size_t>& indices)
{
	destroy();
	
	if (indices.size() % 3 != 0)
	{
		std::cerr << "Navmesh::create(): index count is non multiple of 3\n";
		return false;
	}
	
	// Copy vertices
	this->vertices.resize(vertices.size());
	for (std::size_t i = 0; i < vertices.size(); ++i)
	{
		Navmesh::Vertex* vertex = new Navmesh::Vertex();
		vertex->edge = nullptr;
		vertex->position = vertices[i];
		vertex->flags = 0;
		vertex->index = i;
		
		this->vertices[i] = vertex;
	}
	
	// Allocate triangles
	triangles.resize(indices.size() / 3, nullptr);
	std::size_t currentTriangle = 0;
	
	// Load triangles
	std::map<std::pair<std::size_t, std::size_t>, Navmesh::Edge*> edgeMap;
	for (std::size_t i = 0; i < indices.size(); i += 3)
	{
		std::size_t a = indices[i];
		std::size_t b = indices[i + 1];
		std::size_t c = indices[i + 2];
		
		if (a >= vertices.size() || b >= vertices.size() || c >= vertices.size())
		{
			std::cerr << "Navmesh::create(): Mesh contains invalid index.\n";
			destroy();
			return false;
		}
		
		// Allocate three edges and a triangle
		Navmesh::Edge* ab = new Navmesh::Edge();
		Navmesh::Edge* bc = new Navmesh::Edge();
		Navmesh::Edge* ca = new Navmesh::Edge();
		Navmesh::Triangle* triangle = new Navmesh::Triangle();
		
		// Zero triangle flags
		triangle->flags = 0;
		
		// Zero edge flags
		ab->flags = 0;
		bc->flags = 0;
		ca->flags = 0;

		// Set triangle start edge
		triangle->edge = ab;

		// For each edge in this triangle
		std::size_t triangleIndices[] = {a, b, c};
		Navmesh::Edge* triangleEdges[] = {ab, bc, ca};
		for (std::size_t j = 0; j < 3; ++j)
		{
			// Set edge properties
			Navmesh::Edge* edge = triangleEdges[j];
			edge->triangle = triangle;
			edge->vertex = this->vertices[triangleIndices[j]];
			edge->previous = triangleEdges[(j + 2) % 3];
			edge->next = triangleEdges[(j + 1) % 3];
			edge->symmetric = nullptr;

			// Point vertex to this edge
			edge->vertex->edge = edge;

			// Check for symmetry
			std::pair<std::size_t, std::size_t> symmetricPair(triangleIndices[(j + 1) % 3], triangleIndices[j]);
			std::map<std::pair<std::size_t, std::size_t>, Navmesh::Edge*>::iterator it = edgeMap.find(symmetricPair);
			if (it == edgeMap.end())
			{
				// No symmetric edge found, insert this edge into the map
				std::pair<std::size_t, std::size_t> pair(triangleIndices[j], triangleIndices[(j + 1) % 3]);
				edgeMap[pair] = edge;
			}
			else
			{
				// Symmetric edge found, connect
				edge->symmetric = it->second;
				it->second->symmetric = edge;
			}
		}

		// Set edge indices and add edges to the edge list
		ab->index = edges.size();
		edges.push_back(ab);
		bc->index = edges.size();
		edges.push_back(bc);
		ca->index = edges.size();
		edges.push_back(ca);
		
		// Set triangle index and add triangle to the triangle list
		triangle->index = currentTriangle;
		triangles[currentTriangle++] = triangle;
	}
	
	calculateNormals();
	calculateBounds();
	
	return true;
}

void Navmesh::destroy()
{
	for (std::size_t i = 0; i < vertices.size(); ++i)
		delete vertices[i];
	for (std::size_t i = 0; i < edges.size(); ++i)
		delete edges[i];
	for (std::size_t i = 0; i < triangles.size(); ++i)
		delete triangles[i];
	
	vertices.clear();
	edges.clear();
	triangles.clear();
}

bool Navmesh::loadOBJ(const std::string& filename)
{
	// Open OBJ file
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Navmesh::loadOBJ(): Failed to open Wavefront OBJ file \"" << filename << "\"" << std::endl;
		return false;
	}

	// Read OBJ file from file stream
	if (!readOBJ(&file, filename))
	{
		std::cerr << "Navmesh::loadOBJ(): Failed to read Wavefront OBJ file \"" << filename << "\"" << std::endl;
		file.close();
		return false;
	}

	// Close OBJ file
	file.close();

	return true;
}

void Navmesh::traverse(Navmesh::Triangle* startTriangle, const Vector3& startPosition, const Vector3& startVelocity, std::vector<Navmesh::Step>* traversal)
{
	// Form initial traversal step
	Navmesh::Step step;
	step.triangle = startTriangle;
	step.start = normalize_barycentric(startPosition);
	step.end = step.start;
	step.edge = nullptr;
	
	// Determine the maximum distance of the traversal
	float maxDistance = glm::length(startVelocity);
	
	// Set initial velocity
	Vector3 velocity = startVelocity;
		
	// Traverse navmesh
	float distance = 0.0f;
	while (distance < maxDistance)
	{
		// Grab triangle coordinates
		const Vector3& a = step.triangle->edge->vertex->position;
		const Vector3& b = step.triangle->edge->next->vertex->position;
		const Vector3& c = step.triangle->edge->previous->vertex->position;
		
		// Calculate target position
		Vector3 cartesianStart = cartesian(step.start, a, b, c);
		Vector3 target = cartesianStart + velocity;
		
		// Find closest point on triangle to target position
		closestPointOnTriangle(target, step.triangle, &step.end, &step.edge);
		step.end = normalize_barycentric(step.end);
		
		// Add step to the traversal
		traversal->push_back(step);
		
		// Determine distance traveled by the step
		Vector3 cartesianEnd = cartesian(step.end, a, b, c);
		distance += glm::length(cartesianEnd - cartesianStart);
		
		// Check for no movement
		if (cartesianEnd == cartesianStart)
		{
			/*
			std::cout << "the same!\n";
			
			if (step.edge == nullptr)
				std::cout << "\tand no edge\n";
			else if (step.edge->symmetric == nullptr)
			{
				std::cout << "\tand disconnected\n";
				
				//step.edge = step.edge->previous;
			}
			//break;
			*/
			
		}
		
		// Check if traversal is complete or edge is disconnected
		if (step.edge == nullptr || step.edge->symmetric == nullptr)
		{
			break;
		}
		
		// Recalculate velocity
		Quaternion rotation = glm::rotation(step.triangle->normal, step.edge->symmetric->triangle->normal);
		velocity = glm::normalize(rotation * velocity) * (maxDistance - distance);
		
		// Move to the next triangle
		step.triangle = step.edge->symmetric->triangle;
		
		// Ensure triangle wasn't already visited
		for (const Step& visited: (*traversal))
		{
			if (step.triangle == visited.triangle)
			{
				return;
			}
		}
		
		// Calculate barycentric starting coordinates of the next step
		step.start = normalize_barycentric(barycentric(cartesianEnd,
			step.triangle->edge->vertex->position,
			step.triangle->edge->next->vertex->position,
			step.triangle->edge->previous->vertex->position));
		step.end = step.start;
		step.edge = nullptr;
	}
	
	/*
	// Add triangle to visited list
	visited->push_back(triangle);
	
	// Grab triangle coordinates
	const glm::vec3& a = triangle->edge->vertex->position;
	const glm::vec3& b = triangle->edge->next->vertex->position;
	const glm::vec3& c = triangle->edge->previous->vertex->position;
	
	// Project target onto triangle
	glm::vec3 closestPoint;
	int edgeIndex = -1;
	WingedEdge::Edge* closestEdge = nullptr;
	project_on_triangle(target, a, b, c, &closestPoint, &edgeIndex);
	*end = closestPoint;
	
	// Determine if projected target is on an edge
	switch (edgeIndex)
	{
		case -1:
			// Projected target inside triangle
			return;
		case 0:
			closestEdge = triangle->edge;
			break;
		case 1:
			closestEdge = triangle->edge->next;
			break;
		case 2:
			closestEdge = triangle->edge->previous;
			break;
	}
	
	// If edge is not loose, repeat with connected triangle
	if (closestEdge->symmetric != nullptr)
	{
		for (std::size_t i = 0; i < visited->size() - 1; ++i)
		{
			if ((*visited)[i] == closestEdge->symmetric->triangle)
				return;
		}

		move(mesh, closestEdge->symmetric->triangle, closestPoint, target, visited, end);
	}
	*/
}

/*
		if (steerCCW.isTriggered())
		{
			glm::quat rotation = glm::angleAxis(0.1f, navi.triangle->normal);
			navi_forward = glm::normalize(rotation * navi_forward);
		}
		if (steerCW.isTriggered())
		{
			glm::quat rotation = glm::angleAxis(-0.1f, navi.triangle->normal);
			navi_forward = glm::normalize(rotation * navi_forward);
		}
		
		if (navigate.isTriggered())
		{
			
			Mesh::Triangle* triangle = navi.triangle;
			glm::vec3 start = navi.position;
			glm::vec3 target = start + navi_forward * 0.02f;
			std::vector<Mesh::Triangle*> visited;
			glm::vec3 end;
			
			move(&sceneManager.getTerrain()->mesh, triangle, start, target, &visited, &end);
			
			Mesh::Triangle* end_triangle = visited[visited.size() - 1];
			const glm::vec3& a = end_triangle->edge->vertex->position;
			const glm::vec3& b = end_triangle->edge->next->vertex->position;
			const glm::vec3& c = end_triangle->edge->previous->vertex->position;
			glm::vec3 p = (a + b + c) / 3.0f;
			const glm::vec3& n = end_triangle->normal;
			glm::vec3 projected_start = project_on_plane(start, p, n);
			
			// Calculate difference between positions
			glm::vec3 difference = end - projected_start;
			if (glm::dot(difference, difference) != 0.0f)
			{
				if (end_triangle != triangle)
				{
					glm::quat alignment = glm::rotation(triangle->normal, end_triangle->normal);
					navi_forward = glm::normalize(alignment * navi_forward);
				}
			}
			
			navi.position = end;
			navi.triangle = visited[visited.size() - 1];
		}
*/

void Navmesh::calculateNormals()
{
	for (std::size_t i = 0; i < triangles.size(); ++i)
	{
		Navmesh::Triangle* triangle = triangles[i];

		// Calculate surface normal
		const Vector3& a = triangle->edge->vertex->position;
		const Vector3& b = triangle->edge->next->vertex->position;
		const Vector3& c = triangle->edge->previous->vertex->position;
		Vector3 ba = b - a;
		Vector3 ca = c - a;
		triangle->normal = glm::normalize(glm::cross(ba, ca));
	}
}

void Navmesh::calculateBounds()
{
	Vector3 minPoint(std::numeric_limits<float>::infinity());
	Vector3 maxPoint(-std::numeric_limits<float>::infinity());
	
	for (const Navmesh::Vertex* vertex: vertices)
	{
		minPoint.x = std::min<float>(minPoint.x, vertex->position.x);
		minPoint.y = std::min<float>(minPoint.y, vertex->position.y);
		minPoint.z = std::min<float>(minPoint.z, vertex->position.z);
		
		maxPoint.x = std::max<float>(maxPoint.x, vertex->position.x);
		maxPoint.y = std::max<float>(maxPoint.y, vertex->position.y);
		maxPoint.z = std::max<float>(maxPoint.z, vertex->position.z);
	}
	
	bounds.setMin(minPoint);
	bounds.setMax(maxPoint);
}

bool Navmesh::readOBJ(std::istream* stream, const std::string& filename)
{
	std::string line;
	std::vector<Vector3> vertices;
	std::vector<std::size_t> indices;

	while (stream->good() && std::getline(*stream, line))
	{
		// Tokenize line
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream linestream(line);
		while (linestream >> token)
			tokens.push_back(token);
		
		// Skip empty lines and comments
		if (tokens.empty() || tokens[0][0] == '#')
			continue;
		
		if (tokens[0] == "v")
		{
			if (tokens.size() != 4)
			{
				std::cerr << "Navmesh::readOBJ(): Invalid line \"" << line << "\" in file \"" << filename << "\"" << std::endl;
				return false;
			}
			
			Vector3 vertex;
			
			std::stringstream(tokens[1]) >> vertex.x;
			std::stringstream(tokens[2]) >> vertex.y;
			std::stringstream(tokens[3]) >> vertex.z;

			vertices.push_back(vertex);
		}
		else if (tokens[0] == "f")
		{
			if (tokens.size() != 4)
			{
				std::cerr << "Navmesh::readOBJ(): Invalid line \"" << line << "\" in file \"" << filename << "\"" << std::endl;
				return false;
			}
			
			std::size_t a, b, c;
			std::stringstream(tokens[1]) >> a;
			std::stringstream(tokens[2]) >> b;
			std::stringstream(tokens[3]) >> c;
			
			a -= 1;
			b -= 1;
			c -= 1;
			
			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
		}
	}
	
	return create(vertices, indices);
}

Vector3 Navmesh::barycentric(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
	Vector3 v0 = b - a;
	Vector3 v1 = c - a;
	Vector3 v2 = p - a;

    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

	Vector3 result;
    result.y = (d11 * d20 - d01 * d21) / denom; // v
    result.z = (d00 * d21 - d01 * d20) / denom; // w
    result.x = 1.0f - result.y - result.z;      // u
	
	return result;
}

Vector3 Navmesh::cartesian(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
	return a * p.x + b * p.y + c * p.z;
}

// code taken from Detour's dtClosestPtPointTriangle
// @see https://github.com/recastnavigation/recastnavigation/blob/master/Detour/Source/DetourCommon.cpp
// (zlib license)
void Navmesh::closestPointOnTriangle(const Vector3& p, const Navmesh::Triangle* triangle, Vector3* closestPoint, Navmesh::Edge** closestEdge)
{
	// Grab triangle coordinates
	const Vector3& a = triangle->edge->vertex->position;
	const Vector3& b = triangle->edge->next->vertex->position;
	const Vector3& c = triangle->edge->previous->vertex->position;
	
	// Check if P in vertex region outside A
	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 ap = p - a;
	float d1 = glm::dot(ab, ap);
	float d2 = glm::dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// Barycentric coordinates (1, 0, 0)
		*closestPoint = Vector3(1.0f, 0.0f, 0.0f);
		*closestEdge = triangle->edge;
		return;
	}
	
	// Check if P in vertex region outside B
	Vector3 bp = p - b;
	float d3 = glm::dot(ab, bp);
	float d4 = glm::dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3)
	{
		// Barycentric coordinates (0, 1, 0)
		*closestPoint = Vector3(0.0f, 1.0f, 0.0f);
		*closestEdge = triangle->edge->next;
		return;
	}
	
	// Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		// barycentric coordinates (1-v,v,0)
		float v = d1 / (d1 - d3);
		*closestPoint = Vector3(1.0f - v, v, 0.0f);
		*closestEdge = triangle->edge;
		return;
	}
	
	// Check if P in vertex region outside C
	Vector3 cp = p - c;
	float d5 = glm::dot(ab, cp);
	float d6 = glm::dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6)
	{
		// Barycentric coordinates (0, 0, 1)
		*closestPoint = Vector3(0.0f, 0.0f, 1.0f);
		*closestEdge = triangle->edge->previous;
		return;
	}
	
	// Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		// Barycentric coordinates (1 - w, 0, w)
		float w = d2 / (d2 - d6);
		*closestPoint = Vector3(1.0f - w, 0.0f, w);
		*closestEdge = triangle->edge->previous;
		return;
	}
	
	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		// Barycentric coordinates (0, 1 - w, w)
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closestPoint = Vector3(0.0f, 1.0f - w, w);
		*closestEdge = triangle->edge->next;
		return;
	}
	
	// P inside face region. Compute Q through its barycentric coordinates (u, v, w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closestPoint = Vector3(1.0f - v - w, v, w);
	*closestEdge = nullptr;
}

std::tuple<bool, float, float, float> intersects(const Ray& ray, const Navmesh::Triangle* triangle)
{
	return ray.intersects(triangle->edge->vertex->position, triangle->edge->next->vertex->position, triangle->edge->previous->vertex->position);
}

std::tuple<bool, float, float, std::size_t, std::size_t> intersects(const Ray& ray, const std::list<Navmesh::Triangle*>& triangles)
{
	bool intersection = false;
	float t0 = std::numeric_limits<float>::infinity();
	float t1 = -std::numeric_limits<float>::infinity();
	std::size_t index0 = triangles.size();
	std::size_t index1 = triangles.size();
	
	for (const Navmesh::Triangle* triangle: triangles)
	{
		auto result = intersects(ray, triangle);
		
		if (std::get<0>(result))
		{
			intersection = true;

			float t = std::get<1>(result);
			float cosTheta = glm::dot(ray.direction, triangle->normal);	

			if (cosTheta <= 0.0f)
			{
				// Front-facing
				if (t < t0)
				{
					t0 = t;
					index0 = triangle->index;
				}

			}
			else
			{
				// Back-facing
				if (t > t1)
				{
					t1 = t;
					index1 = triangle->index;
				}
			}
		}
	}

	return std::make_tuple(intersection, t0, t1, index0, index1);
}

std::tuple<bool, float, float, std::size_t, std::size_t> intersects(const Ray& ray, const Navmesh& mesh)
{
	const std::vector<Navmesh::Triangle*>& triangles = *mesh.getTriangles();
	
	bool intersection = false;
	float t0 = std::numeric_limits<float>::infinity();
	float t1 = -std::numeric_limits<float>::infinity();
	std::size_t index0 = triangles.size();
	std::size_t index1 = triangles.size();

	for (std::size_t i = 0; i < triangles.size(); ++i)
	{
		const Navmesh::Triangle* triangle = triangles[i];
		const Vector3& a = triangle->edge->vertex->position;
		const Vector3& b = triangle->edge->next->vertex->position;
		const Vector3& c = triangle->edge->previous->vertex->position;
		
		auto result = ray.intersects(a, b, c);
		
		if (std::get<0>(result))
		{
			intersection = true;

			float t = std::get<1>(result);
			float cosTheta = glm::dot(ray.direction, triangle->normal);	

			if (cosTheta <= 0.0f)
			{
				// Front-facing
				t0 = std::min<float>(t0, t);
				index0 = i;
			}
			else
			{
				// Back-facing
				t1 = std::max<float>(t1, t);
				index1 = i;
			}
		}
	}

	return std::make_tuple(intersection, t0, t1, index0, index1);
}

Octree<Navmesh::Triangle*>* Navmesh::createOctree(std::size_t maxDepth)
{
	Octree<Navmesh::Triangle*>* result = new Octree<Navmesh::Triangle*>(maxDepth, bounds);
	
	for (Navmesh::Triangle* triangle: triangles)
	{
		Vector3 min;
		min.x = std::min<float>(triangle->edge->vertex->position.x, std::min<float>(triangle->edge->next->vertex->position.x, triangle->edge->previous->vertex->position.x));
		min.y = std::min<float>(triangle->edge->vertex->position.y, std::min<float>(triangle->edge->next->vertex->position.y, triangle->edge->previous->vertex->position.y));
		min.z = std::min<float>(triangle->edge->vertex->position.z, std::min<float>(triangle->edge->next->vertex->position.z, triangle->edge->previous->vertex->position.z));
		
		Vector3 max;
		max.x = std::max<float>(triangle->edge->vertex->position.x, std::max<float>(triangle->edge->next->vertex->position.x, triangle->edge->previous->vertex->position.x));
		max.y = std::max<float>(triangle->edge->vertex->position.y, std::max<float>(triangle->edge->next->vertex->position.y, triangle->edge->previous->vertex->position.y));
		max.z = std::max<float>(triangle->edge->vertex->position.z, std::max<float>(triangle->edge->next->vertex->position.z, triangle->edge->previous->vertex->position.z));
		
		result->insert(AABB(min, max), triangle);
	}
	
	return result;
}

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

#include "mesh.hpp"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <limits>

bool loadHeightmap(const std::string& filename, glm::vec3 scale, WingedEdge* mesh)
{
	int width;
	int height;
	int components;
	
	// Load image data
	unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &components, 1);
	if (!pixels)
	{
		std::cerr << "Failed to load heightmap image \"" << filename << "\"\n";
		return false;
	}
	
	std::size_t vertexCount = width * height;
	std::size_t triangleCount = (width - 1) * (height - 1) * 2;
	std::size_t indexCount = triangleCount * 3;
	
	std::vector<glm::vec3> vertices(vertexCount);
	std::vector<std::size_t> indices(indexCount);
	
	// Adjust scale
	scale.x *= 1.0f / ((float)width - 1);
	scale.y *= 1.0f / 255.0f;
	scale.z *= 1.0f / ((float)height - 1);
	if (width > height) scale.z *= (float)height / (float) width;
	else if (height > width) scale.x *= (float)width / (float)height;
	
	// Calculate centered offset
	glm::vec3 offset;
	offset.x = (float)width * -0.5f * scale.x;
	offset.y = 0.0f;
	offset.z = (float)height * -0.5f * scale.z;
	
	// Calculate vertex positions
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			std::size_t index = i * width + j;
			
			glm::vec3* vertex = &vertices[index];
			vertex->x = (float)j * scale.x + offset.x;
			vertex->y = (float)pixels[index] * scale.y;
			vertex->z = (float)i * scale.z + offset.z;
		}
	}
	
	// Free loaded image
	stbi_image_free(pixels);
	
	// Generate indices
	for (int i = 0; i < height - 1; ++i)
	{
		for (int j = 0; j < width - 1; ++j)
		{
			std::size_t a = i * width + j;
			std::size_t b = (i + 1) * width + j;
			std::size_t c = i * width + j + 1;
			std::size_t d = (i + 1) * width + j + 1;
			
			std::size_t index = (i * (width - 1) + j) * 2 * 3;
			indices[index] = a;
			indices[index + 1] = b;
			indices[index + 2] = c;
			indices[index + 3] = c;
			indices[index + 4] = b;
			indices[index + 5] = d;
		}
	}
	
	return mesh->create(vertices, indices);
}

bool loadHeightmapBase(const std::string& filename, glm::vec3 scale, float floor, WingedEdge* mesh)
{
	int width;
	int height;
	int components;
	
	// Load image data
	unsigned char* pixels = stbi_load(filename.c_str(), &width, &height, &components, 1);
	if (!pixels)
	{
		std::cerr << "Failed to load heightmap image \"" << filename << "\"\n";
		return false;
	}
	
	std::size_t vertexCount = width * 4 + height * 4;
	std::size_t triangleCount = (width - 1) * 4 + (height - 1) * 4;
	std::size_t indexCount = triangleCount * 3;
	
	std::vector<glm::vec3> vertices(vertexCount);
	std::vector<std::size_t> indices(indexCount);
	
	// Adjust scale
	scale.x *= 1.0f / ((float)width - 1);
	scale.y *= 1.0f / 255.0f;
	scale.z *= 1.0f / ((float)height - 1);
	if (width > height) scale.z *= (float)height / (float) width;
	else if (height > width) scale.x *= (float)width / (float)height;
	
	// Calculate centered offset
	glm::vec3 offset;
	offset.x = (float)width * -0.5f * scale.x;
	offset.y = 0.0f;
	offset.z = (float)height * -0.5f * scale.z;
	
	glm::vec3* vertex = &vertices[0];
	
	// Top row
	for (int j = 0; j < width; ++j)
	{
		int i = 0;
		std::size_t index = i * width + j;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = (float)pixels[index] * scale.y;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = floor;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
	}
	
	// Bottom row
	for (int j = 0; j < width; ++j)
	{
		int i = (height - 1);
		std::size_t index = i * width + j;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = (float)pixels[index] * scale.y;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = floor;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
	}
	
	// Left column
	for (int i = 0; i < height; ++i)
	{
		int j = 0;
		std::size_t index = i * width + j;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = (float)pixels[index] * scale.y;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = floor;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
	}
	
	// Right column
	for (int i = 0; i < height; ++i)
	{
		int j = (width - 1);
		std::size_t index = i * width + j;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = (float)pixels[index] * scale.y;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
		
		vertex->x = (float)j * scale.x + offset.x;
		vertex->y = floor;
		vertex->z = (float)i * scale.z + offset.z;
		++vertex;
	}
	
	// Free loaded image
	stbi_image_free(pixels);
	
	// Generate indices
	std::size_t* index = &indices[0];
	
	for (int i = 0; i < width - 1; ++i)
	{
		std::size_t a = i * 2;
		std::size_t b = i * 2 + 1;
		std::size_t c = (i + 1) * 2;
		std::size_t d = (i + 1) * 2 + 1;
		
		(*(index++)) = b;
		(*(index++)) = a;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = d;
		
		a += width * 2;
		b += width * 2;
		c += width * 2;
		d += width * 2;
		
		(*(index++)) = a;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = d;
	}
	
	for (int i = 0; i < height - 1; ++i)
	{
		std::size_t a = width * 4 + i * 2;
		std::size_t b = width * 4 + i * 2 + 1;
		std::size_t c = width * 4 + (i + 1) * 2;
		std::size_t d = width * 4 + (i + 1) * 2 + 1;
		
		(*(index++)) = a;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = d;
		
		a += height * 2;
		b += height * 2;
		c += height * 2;
		d += height * 2;
		
		(*(index++)) = b;
		(*(index++)) = a;
		(*(index++)) = c;
		(*(index++)) = b;
		(*(index++)) = c;
		(*(index++)) = d;
	}
	
	return mesh->create(vertices, indices);
}

void move(const WingedEdge* mesh, WingedEdge::Triangle* triangle, const glm::vec3& start, const glm::vec3& target, std::vector<WingedEdge::Triangle*>* visited, glm::vec3* end)
{
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
}

std::tuple<bool, float, float, float> intersects(const glm::vec3& origin, const glm::vec3& direction, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	// Find edges
	glm::vec3 edge10 = b - a;
	glm::vec3 edge20 = c - a;

	// Calculate determinant
	glm::vec3 pv = glm::cross(direction, edge20);
	float det = glm::dot(edge10, pv);
	if (!det)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
	}
	float inverseDet = 1.0f / det;

	// Calculate u
	glm::vec3 tv = origin - a;
	float u = glm::dot(tv, pv) * inverseDet;
	
	if (u < 0.0f || u > 1.0f)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
	}

	// Calculate v
	glm::vec3 qv = glm::cross(tv, edge10);
	float v = glm::dot(direction, qv) * inverseDet;

	if (v < 0.0f || u + v > 1.0f)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
	}

	// Calculate t
	float t = glm::dot(edge20, qv) * inverseDet;

	if (t > 0.0f)
	{
		return std::make_tuple(true, t, u, v);
	}

	return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
}

std::tuple<bool, float, float, std::size_t, std::size_t> intersects(const glm::vec3& origin, const glm::vec3& direction, const WingedEdge& mesh)
{
	const std::vector<WingedEdge::Triangle*>* triangles = mesh.getTriangles();
	bool intersection = false;
	float t0 = std::numeric_limits<float>::infinity();
	float t1 = -std::numeric_limits<float>::infinity();
	std::size_t index0 = triangles->size();
	std::size_t index1 = index0;

	for (std::size_t i = 0; i < triangles->size(); ++i)
	{
		const WingedEdge::Triangle* triangle = (*triangles)[i];
		const glm::vec3& a = triangle->edge->vertex->position;
		const glm::vec3& b = triangle->edge->next->vertex->position;
		const glm::vec3& c = triangle->edge->previous->vertex->position;

		auto result = intersects(origin, direction, a, b, c);
		if (std::get<0>(result))
		{
			intersection = true;

			float t = std::get<1>(result);
			float cosTheta = glm::dot(direction, triangle->normal);

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

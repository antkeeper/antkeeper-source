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

#ifndef NAVMESH_HPP
#define NAVMESH_HPP

#include <iostream>
#include <string>
#include <vector>

#include <emergent/emergent.hpp>
using namespace Emergent;

/**
 * Navigation mesh represented by a half-edge structure.
 *
 * @ingroup geometry
 */
class Navmesh
{
public:
	struct Vertex;
	struct Edge;
	struct Triangle;
	struct Step;
	
	/**
	 * Creates an instance of Navmesh.
	 */
	Navmesh();
	
	/**
	 * Destroys an instance of Navmesh.
	 */
	~Navmesh();
	
	/**
	 * Forms a navmesh from a list of vertices and indices.
	 *
	 * @param vertices Specifies a list of vertices.
	 * @param indices Specifies a list of indices.
	 * @return `true` if the navmesh was successfully created, `false` otherwise.
	 */
	bool create(const std::vector<Vector3>& vertices, const std::vector<std::size_t>& indices);
	
	/**
	 * Destroys the navmesh.
	 */
	void destroy();
	
	/**
	 * Loads this navmesh from a triangulated Wavefront OBJ file. This method only supported **triangulated** Wavefront OBJ files. The supported commands are `v`, `f` and comment lines beginning with `#`.
	 *
	 * @param filename Path to the Wavefront OBJ file.
	 * @return `true` if the navmesh was successfully loaded from the OBJ file, `false` otherwise.
	 */
	bool loadOBJ(const std::string& filename);
	
	/**
	 * Traverses the navmesh.
	 *
	 * @param[in] startTriangle Initial triangle
	 * @param[in] startPosition Initial barycentric coordinates on the start triangle
	 * @param[in] startVelocity Initial cartesian velocity vector
	 * @param[out] traversal Traversal information
	 */
	static void traverse(Navmesh::Triangle* startTriangle, const Vector3& startPosition, const Vector3& startVelocity, std::vector<Navmesh::Step>* traversal);
	
	/// Returns a pointer to the navmesh vertices
	const std::vector<Navmesh::Vertex*>* getVertices() const;
	
	/// Returns a pointer to the navmesh edges
	const std::vector<Navmesh::Edge*>* getEdges() const;
	
	/// Returns a pointer to the navmesh triangles
	const std::vector<Navmesh::Triangle*>* getTriangles() const;
	
	/// @copydoc Navmesh::getVertices() const
	std::vector<Navmesh::Vertex*>* getVertices();
	
	/// @copydoc Navmesh::getEdges() const
	std::vector<Navmesh::Edge*>* getEdges();
	
	/// @copydoc Navmesh::getTriangles() const
	std::vector<Navmesh::Triangle*>* getTriangles();
	
	/**
	 * Half-edge vertex which contains a pointer to its parent edge, a position vector, and an index.
	 */
	struct Vertex
	{
		/// Pointer to the edge to which this vertex belongs
		Navmesh::Edge* edge;
		
		/// Vertex position vector
		Vector3 position;
		
		/// Vertex flags
		unsigned char flags;
		
		/// Index of this vertex
		std::size_t index;
	};
	
	/**
	 * Half-edge edge which contains pointers to its starting vertex, parent triangle, and related edges.
	 */
	struct Edge
	{
		/// Pointer to the vertex at which the edge starts
		Navmesh::Vertex* vertex;     
		
		/// Pointer to the triangle to which this edge belongs
		Navmesh::Triangle* triangle;
		
		/// Pointer to the previous edge in the parent triangle
		Navmesh::Edge* previous;
		
		/// Pointer to the next edge in the parent triangle
		Navmesh::Edge* next;
		
		/// Pointer to the symmetric edge
		Navmesh::Edge* symmetric;
		
		/// Edge flags
		unsigned char flags;
		
		/// Index of this edge
		std::size_t index;
	};
	
	/**
	 * Half-edge triangle which contains a pointer to its first edge and its normal vector.
	 */
	struct Triangle
	{
		/// Pointer to the first edge in this triangle
		Navmesh::Edge* edge;
		
		/// Faceted surface normal
		Vector3 normal;
		
		/// Triangle flags
		unsigned char flags;
		
		/// Index of this triangle
		std::size_t index;
	};
	
	/**
	 * Contains informations about a single step in a navmesh traversal operation.
	 */
	struct Step
	{
		/// Pointer to the triangle on which the step occured
		Triangle* triangle;
		
		/// Barycentric coordinates of the step's starting position
		Vector3 start;
		
		/// Barycentric coordinates of the step's ending position
		Vector3 end;
		
		/// Pointer to the edge on which the step exited the triangle, or `nullptr` if the step is within the triangle
		Edge* edge;
	};
	
	/**
	 * Calculates the faceted surface normals for each triangle.
	 */
	void calculateNormals();
	
private:
	/**
	 * Reads Wavefront OBJ data from an input stream
	 *
	 * @param stream Input stream containing OBJ data
	 * @param filename Path to the OBJ file
	 * @return `true` if OBJ data was successfully read from the file.
	 */
	bool readOBJ(std::istream* stream, const std::string& filename);
	
	/**
	 * Calculates barycentric coordinates from cartesian coordinates.
	 *
	 * @param p Cartesian point
	 * @param a First vertex in triangle
	 * @param b Second vertex in triangle
	 * @param c Third vertex in triangle
	 */
	static Vector3 barycentric(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);
	
	/**
	 * Calculates cartesian coordinates from barycentric coordinates.
	 *
	 * @param p Barycentric point
	 * @param a First vertex in triangle
	 * @param b Second vertex in triangle
	 * @param c Third vertex in triangle
	 */
	static Vector3 cartesian(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c);
	
	/**
	 * Finds the closest point on a triangle.
	 *
	 * @param[in] p Point to project
	 * @param[in] triangle Triangle on which to find the closest point
	 * @param[out] closest Closest point on triangle
	 * @param[out] edge Edge on which the closest point is located, or `nullptr` if the closest point is not on an edge.
	 */
	static void closestPointOnTriangle(const Vector3& p, const Navmesh::Triangle* triangle, Vector3* closestPoint, Navmesh::Edge** closestEdge);
	
	std::vector<Navmesh::Vertex*> vertices;
	std::vector<Navmesh::Edge*> edges;
	std::vector<Navmesh::Triangle*> triangles;
};

inline const std::vector<Navmesh::Vertex*>* Navmesh::getVertices() const
{
	return &vertices;
}

inline const std::vector<Navmesh::Edge*>* Navmesh::getEdges() const
{
	return &edges;
}

inline const std::vector<Navmesh::Triangle*>* Navmesh::getTriangles() const
{
	return &triangles;
}

inline std::vector<Navmesh::Vertex*>* Navmesh::getVertices()
{
	return &vertices;
}

inline std::vector<Navmesh::Edge*>* Navmesh::getEdges()
{
	return &edges;
}

inline std::vector<Navmesh::Triangle*>* Navmesh::getTriangles()
{
	return &triangles;
}

#endif // NAVMESH_HPP

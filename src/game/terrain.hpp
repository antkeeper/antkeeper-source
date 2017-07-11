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

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "navmesh.hpp"
#include "../materials.hpp"
#include <emergent/emergent.hpp>

using namespace Emergent;

class Terrain
{
public:
	Terrain();
	~Terrain();
	
	/**
	 * Creates a flat terrain surface.
	 *
	 * @param columns Specifies the width of the terrain, in cells.
	 * @param rows Specifies the depth of the terrain, in cells.
	 * @param dimensions Specifies the dimensions of the terrain.
	 */
	void create(int columns, int rows, const Vector3& dimensions);
	
	/// Loads a heightmap
	bool load(const std::string& filename);
	
	/// Returns the navmesh representing the terrain surface.
	const Navmesh* getSurfaceNavmesh() const;
	
	/// Returns the navmesh representing the terrain surface.
	Navmesh* getSurfaceNavmesh();
	
	/// Returns the navmesh representing the terrain subsurface.
	const Navmesh* getSubsurfaceNavmesh() const;
	
	/// Returns the navmesh representing the terrain subsurface.
	Navmesh* getSubsurfaceNavmesh();
	
	/// Returns the model representing the terrain surface.
	const Model* getSurfaceModel() const;
	
	/// Returns the model representing the terrain surface.
	Model* getSurfaceModel();
	
	/// Returns the model representing the terrain subsurface.
	const Model* getSubsurfaceModel() const;
	
	/// Returns the model representing the terrain subsurface.
	Model* getSubsurfaceModel();
	
	const Octree<Navmesh::Triangle*>* getSurfaceOctree() const;
	
private:
	void createSurface();
	void createSubsurface();
	
	void calculateSurfaceNormals();
	
	int columns;
	int rows;
	Vector3 dimensions;
	
	// Surface
	std::size_t surfaceVertexSize;
	std::size_t surfaceVertexCount;
	std::size_t surfaceTriangleCount;
	std::size_t surfaceIndexCount;
	float* surfaceVertexData;
	std::uint32_t* surfaceIndexData;
	std::vector<Vector3> surfaceVertices;
	std::vector<std::size_t> surfaceIndices;
	GLuint surfaceVAO;
	GLuint surfaceVBO;
	GLuint surfaceIBO;
	PhysicalMaterial surfaceMaterial;
	Model surfaceModel;
	Navmesh surfaceNavmesh;
	Octree<Navmesh::Triangle*>* surfaceOctree;
	
	// Subsurface
	std::size_t subsurfaceVertexSize;
	std::size_t subsurfaceVertexCount;
	std::size_t subsurfaceTriangleCount;
	std::size_t subsurfaceIndexCount;
	float* subsurfaceVertexData;
	std::uint32_t* subsurfaceIndexData;
	std::vector<Vector3> subsurfaceVertices;
	std::vector<std::size_t> subsurfaceIndices;
	GLuint subsurfaceVAO;
	GLuint subsurfaceVBO;
	GLuint subsurfaceIBO;
	PhysicalMaterial subsurfaceMaterial;
	Model subsurfaceModel;
	Navmesh subsurfaceNavmesh;
};

inline Navmesh* Terrain::getSurfaceNavmesh()
{
	return &surfaceNavmesh;
};

inline const Navmesh* Terrain::getSurfaceNavmesh() const
{
	return &surfaceNavmesh;
};

inline Navmesh* Terrain::getSubsurfaceNavmesh()
{
	return &subsurfaceNavmesh;
};

inline const Navmesh* Terrain::getSubsurfaceNavmesh() const
{
	return &subsurfaceNavmesh;
};

inline const Model* Terrain::getSurfaceModel() const
{
	return &surfaceModel;
}

inline Model* Terrain::getSurfaceModel()
{
	return &surfaceModel;
}

inline const Model* Terrain::getSubsurfaceModel() const
{
	return &subsurfaceModel;
}

inline Model* Terrain::getSubsurfaceModel()
{
	return &subsurfaceModel;
}

inline const Octree<Navmesh::Triangle*>* Terrain::getSurfaceOctree() const
{
	return surfaceOctree;
}

#endif // TERRAIN_HPP

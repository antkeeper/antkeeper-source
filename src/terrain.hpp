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

#include <emergent/emergent.hpp>

using namespace Emergent;

class Terrain
{
public:
	/**
	 * Creates a flat terrain surface.
	 *
	 * @param columns Specifies the width of the terrain, in cells.
	 * @param rows Specifies the depth of the terrain, in cells.
	 * @param dimensions Specifies the dimensions of the terrain.
	 */
	void create(int columns, int rows, const Vector3& dimensions);
	
	/// Returns the winged-edge mesh representing the terrain surface.
	const WingedEdge* getSurfaceMesh() const;
	
	/// Returns the winged-edge mesh representing the terrain subsurface.
	const WingedEdge* getSubsurfaceMesh() const;
	
	/// Returns the model representing the terrain surface.
	const Model* getSurfaceModel() const;
	
	/// Returns the model representing the terrain subsurface.
	const Model* getSubsurfaceModel() const;
	
private:
	void createSurface();
	void createSubsurface();
	
	int columns;
	int rows;
	Vector3 dimensions;
	std::vector<Vector3> surfaceVertices;
	std::vector<Vector3> subsurfaceVertices;
	std::vector<std::size_t> surfaceIndices;
	std::vector<std::size_t> subsurfaceIndices;
	WingedEdge surfaceMesh;
	WingedEdge subsurfaceMesh;
	Model surfaceModel;
	Model subsurfaceModel;
};

inline const WingedEdge* Terrain::getSurfaceMesh() const
{
	return &surfaceMesh;
};

inline const WingedEdge* Terrain::getSubsurfaceMesh() const
{
	return &subsurfaceMesh;
};

inline const Model* Terrain::getSurfaceModel() const
{
	return &surfaceModel;
}

inline const Model* Terrain::getSubsurfaceModel() const
{
	return &subsurfaceModel;
}

#endif // TERRAIN_HPP

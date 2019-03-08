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

#ifndef TERRAIN_SYSTEM_HPP
#define TERRAIN_SYSTEM_HPP

#include "../entity-group.hpp"
#include "../components/model-component.hpp"
#include "../components/terrain-patch-component.hpp"
#include "../components/transform-component.hpp"
#include "../system.hpp"
#include <map>

// The terrain system creates a model and adds the model component to new members of this group.
typedef EntityGroup<TerrainPatchComponent, TransformComponent> TerrainCreationGroup;
typedef EntityGroup<ModelComponent, TerrainPatchComponent, TransformComponent> TerrainGroup;

class TerrainSystem:
	public System,
	public TerrainCreationGroup::Observer,
	public TerrainGroup::Observer
{
public:
	TerrainSystem(ComponentManager* componentManager);
	virtual ~TerrainSystem();
	
	virtual void update(float t, float dt);

private:
	virtual void memberRegistered(const TerrainCreationGroup::Member* member);
	virtual void memberUnregistered(const TerrainCreationGroup::Member* member);
	virtual void memberRegistered(const TerrainGroup::Member* member);
	virtual void memberUnregistered(const TerrainGroup::Member* member);

	/**
	 * Generates a subdivided plane triangle mesh.
	 *
	 * 0 subdivisions:
	 * +---+
	 * | / |
	 * +---+
	 *
	 * 1 subdivision:
	 * +---+---+
	 * | / | / |
	 * +---+---+
	 * | / | / |
	 * +---+---+
	 *
	 * 2 subdivisions:
	 * +---+---+---+---+
	 * | / | / | / | / |
	 * +---+---+---+---+
	 * | / | / | / | / |
	 * +---+---+---+---+
	 * | / | / | / | / |
	 * +---+---+---+---+
	 * | / | / | / | / |
	 * +---+---+---+---+
	 *
	 * @param subdivisions Number of subdivisions.
	 * @return Generated triangle mesh.
	 */
	TriangleMesh* generatePlane(int subdivisions);

	/**
	 * Generates a model from a triangle mesh.
	 *
	 * @param mesh Triangle mesh from which a model will be generated.
	 * @return Generated model.
	 */
	Model* generateModel(TriangleMesh* mesh);

	/**
	 * Projects the vertices of a triangle mesh onto terrain brush geometry.
	 */
	void projectMesh(TriangleMesh* mesh);

	TerrainCreationGroup terrainCreationGroup;
	TerrainGroup terrainGroup;

	std::map<std::tuple<int, int>, const TerrainGroup::Member*> patchMap;
	float patchSize;
};

#endif // TERRAIN_SYSTEM_HPP


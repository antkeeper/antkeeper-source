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

#ifndef COLONY_HPP
#define COLONY_HPP

#include <vector>
#include "navmesh.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

class Ant;
class Agent;
class Pheromone;
class Gait;

/**
 * A colony of ants.
 */
class Colony
{
public:
	Colony();
	~Colony();
	
	Ant* spawn(Navmesh* navmesh, Navmesh::Triangle* triangle, const Vector3& position);
	
	void update(float dt);
	
	void setAntModel(Model* model);
	const Model* getAntModel() const;
	Model* getAntModel();
	
	void queryAnts(const BoundingVolume& volume, std::list<Agent*>* results) const;
	
	std::size_t getAntCount() const;
	const Ant* getAnt(std::size_t index) const;
	Ant* getAnt(std::size_t index);
	
	
	const Octree<Agent*>* getAntOctree() const;
	const Octree<Pheromone*>* getPheromoneOctree() const;
	
private:
	// Rendering
	Model* antModel;
	
	// Locomotion
	float walkSpeed;
	float turnSpeed;
	Gait* tripodGait;
	Gait* rippleGait;
	Gait* slowWaveGait;
	
	
	std::vector<Ant*> ants;
	Octree<Agent*>* antOctree;
	
	std::vector<Pheromone*> pheromones;
	Octree<Pheromone*>* pheromoneOctree;
};

inline const Model* Colony::getAntModel() const
{
	return antModel;
}

inline Model* Colony::getAntModel()
{
	return antModel;
}

inline std::size_t Colony::getAntCount() const
{
	return ants.size();
}

inline const Ant* Colony::getAnt(std::size_t index) const
{
	return ants[index];
}

inline Ant* Colony::getAnt(std::size_t index)
{
	return ants[index];
}


inline const Octree<Agent*>* Colony::getAntOctree() const
{
	return antOctree;
}

inline const Octree<Pheromone*>* Colony::getPheromoneOctree() const
{
	return pheromoneOctree;
}

#endif // COLONY_HPP
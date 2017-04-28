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

#ifndef HABITAT_HPP
#define HABITAT_HPP

#include <vector>

#include <emergent/emergent.hpp>
using namespace Emergent;

class Navmesh;
class Pheromone;
class Agent;

class Habitat
{
public:
	Habitat(const AABB& bounds, int maxOctreeDepth);
	~Habitat();
	
	const Octree<Navmesh*>* getObstacleOctree() const;
	const Octree<Pheromone*>* getPheromoneOctree() const;
	const Octree<Agent*>* getAgentOctree() const;
	
private:
	Octree<Navmesh*>* obstacleOctree;
	Octree<Pheromone*>* pheromoneOctree;
	Octree<Agent*>* agentOctree;
};

#endif // HABITAT_HPP
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

#include "colony.hpp"
#include "ant.hpp"
#include "pheromone.hpp"

Colony::Colony():
	antModel(nullptr)
{
	antOctree = new Octree<Agent*>(AABB(Vector3(-8.0f), Vector3(8.0f)), 5);
	pheromoneOctree = new Octree<Pheromone*>(AABB(Vector3(-8.0f), Vector3(8.0f)), 5);
}

Colony::~Colony()
{
	delete antOctree;
}

Ant* Colony::spawn(Navmesh* navmesh, Navmesh::Triangle* triangle, const Vector3& position)
{
	// Allocate ant
	Ant* ant = new Ant(this);
	
	// Position it on the navmesh
	ant->setPosition(triangle, position);
	
	// Add ant to the colony
	ants.push_back(ant);
	
	return ant;
}

void Colony::update(float dt)
{
	// Rebuild octree
	antOctree->clear();
	for (Ant* ant: ants)
	{
		antOctree->insert(AABB(ant->getPosition(), ant->getPosition()), ant);
	}
	
	// Update ants
	for (Ant* ant: ants)
	{
		ant->update(dt);
	}
}

void Colony::setAntModel(Model* model)
{
	this->antModel = model;
}

void Colony::queryAnts(const BoundingVolume& volume, std::list<Agent*>* results) const
{
	antOctree->query(volume, results);
}

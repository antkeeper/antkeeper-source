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
#include "../configuration.hpp"

Colony::Colony():
	antModel(nullptr),
	tripodGaitAnimation(nullptr)
{
	Vector3 octreeMin = Vector3(-ANTKEEPER_TERRAIN_WIDTH, -ANTKEEPER_TERRAIN_BASE_HEIGHT, -ANTKEEPER_TERRAIN_DEPTH) * 0.5f - Vector3(ANTKEEPER_OCTREE_PADDING);
	Vector3 octreeMax = Vector3( ANTKEEPER_TERRAIN_WIDTH,  ANTKEEPER_TERRAIN_BASE_HEIGHT,  ANTKEEPER_TERRAIN_DEPTH) * 0.5f + Vector3(ANTKEEPER_OCTREE_PADDING);
	AABB octreeBounds(octreeMin, octreeMax);
	
	antOctree = new Octree<Agent*>(5, octreeBounds);
	pheromoneOctree = new Octree<Pheromone*>(5, octreeBounds);
}

Colony::~Colony()
{
	killAll();
	delete antOctree;
	delete pheromoneOctree;
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
		antOctree->insert(ant->getModelInstance()->getBounds(), ant);
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
	
	// Find tripod gait animation
	tripodGaitAnimation = model->getSkeleton()->getAnimation("tripod-gait");
	if (!tripodGaitAnimation)
	{
		std::cerr << "Ant tripod gait animation not found" << std::endl;
	}
}

void Colony::queryAnts(const BoundingVolume& volume, std::list<Agent*>* results) const
{
	antOctree->query(volume, results);
}

void Colony::killAll()
{
	antOctree->clear();
	pheromoneOctree->clear();
	
	for (Ant* ant: ants)
	{
		delete ant;
	}
	ants.clear();
}

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

#include "ant.hpp"

Ant::Ant(Colony* colony):
	colony(colony),
	state(Ant::State::IDLE),
	transform(Transform::getIdentity()),
	skeletonPose(nullptr)
{
	modelInstance.setModel(colony->getAntModel());
}

void Ant::move(const Vector3& velocity)
{
	std::vector<Navmesh::Step> traversal;
	Navmesh::traverse(getNavmeshTriangle(), getBarycentricPosition(), velocity, &traversal);
	
	if (!traversal.empty())
	{
		const Navmesh::Step& step = traversal.back();
		
		if (step.start != step.end)
		{
			if (step.triangle != getNavmeshTriangle())
			{
				Quaternion alignment = glm::rotation(getNavmeshTriangle()->normal, step.triangle->normal);
				Vector3 newForward = glm::normalize(project_on_plane(alignment * getForward(), Vector3(0.0f), step.triangle->normal));
				
				setOrientation(newForward, step.triangle->normal);
			}
		}
		
		setPosition(step.triangle, step.end);
	}
}

void Ant::turn(float angle)
{
	// Rotate forward vector
	Vector3 newForward = glm::normalize(glm::angleAxis(angle, getUp()) * getForward());
	setOrientation(newForward, getUp());
}

void Ant::update(float dt)
{
	if (state == Ant::State::WANDER)
	{
		setWanderCircleDistance(3.0f);
		setWanderCircleRadius(0.25f);
		setWanderRate(glm::radians(90.0f));
		
		// Calculate wander force
		Vector3 wanderForce = wander(dt);
		
		// Setup containment probes
		float probeLateralOffset = 0.35f;
		Vector3 forwardProbe = getForward() * 0.5f;
		Vector3 leftProbe = getForward() * 0.1f - getRight() * probeLateralOffset;
		Vector3 rightProbe = getForward() * 0.1f + getRight() * probeLateralOffset;
		
		// Calculate containment force
		Vector3 containmentForce = containment(forwardProbe)
			+ containment(leftProbe)
			+ containment(rightProbe);
		
		// Calculate velocity
		Vector3 velocity(0.0f);
		velocity += wanderForce;
		velocity += containmentForce;
		velocity = limit(velocity, 0.025f);
		
		setOrientation(glm::normalize(velocity), getUp());
		
		// Move ant
		move(velocity);
	}
	
	// Update transform
	transform.translation = getPosition();
	transform.rotation = getRotation();
	
	// Update model instance
	modelInstance.setTransform(transform);
}

void Ant::setState(Ant::State state)
{
	this->state = state;
}

Colony::Colony():
	antModel(nullptr)
{}

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
	for (Ant* ant: ants)
	{
		ant->update(dt);
	}
}

void Colony::setAntModel(Model* model)
{
	this->antModel = model;
}

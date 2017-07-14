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
#include "colony.hpp"
#include "pheromone.hpp"

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
	float probeLateralOffset = 0.1f;
	float probeForwardOffset = 0.3f;
	
	// Steering
	if (state == Ant::State::WANDER)
	{
		setWanderCircleDistance(4.0f);
		setWanderCircleRadius(0.3f);
		setWanderRate(glm::radians(90.0f));
		setSeparationRadius(0.5f);
		setMaxSpeed(0.025f);
		
		// Calculate wander force
		Vector3 wanderForce = wander(dt);
		
		// Setup containment probes
		Vector3 leftProbe = getForward() * probeForwardOffset - getRight() * probeLateralOffset;
		Vector3 rightProbe = getForward() * probeForwardOffset + getRight() * probeLateralOffset;
		
		// Calculate containment force
		Vector3 containmentForce = containment(leftProbe) + containment(rightProbe);
		
		// Determine neighbors
		float neighborhoodSize = 2.0f;
		AABB neighborhoodAABB(getPosition() - Vector3(neighborhoodSize * 0.5f), getPosition() + Vector3(neighborhoodSize * 0.5f));
		std::list<Agent*> neighbors;
		colony->queryAnts(neighborhoodAABB, &neighbors);
				
		// Calculate separation force
		Vector3 separationForce = separation(neighbors);
		
		// Calculate velocity
		Vector3 velocity = getVelocity();
		velocity += wanderForce;
		velocity += containmentForce * 0.0025f;
		velocity += separationForce * 0.01f;
		velocity = limit(velocity, 0.025f);
		setVelocity(velocity);
		
		setOrientation(glm::normalize(velocity), getUp());
		
		// Move ant
		move(velocity);
	}
	else if (state == Ant::State::IDLE)
	{
		Vector3 leftProbe = getForward() * probeForwardOffset - getRight() * probeLateralOffset;
		Vector3 rightProbe = getForward() * probeForwardOffset + getRight() * probeLateralOffset;
		Vector3 containmentForce = containment(leftProbe) + containment(rightProbe);
		Vector3 velocity = Vector3(0.0f);
		velocity += containmentForce;
		velocity = limit(velocity, 0.025f);
		setVelocity(velocity);
		//setOrientation(glm::normalize(velocity), getUp());
		
		// Move ant
		move(velocity);
	}
	
	// Locomotion
	
	/*
	As the ant moves forward, legs in the stance phase are kept grounded via IK. If IK constraints are violated, the swinging legs are grounded
	and the grounded legs begin swinging.
	
	Two poses are loaded from the model file: midswing and touchdown.
	
	touchdown is the pose in which all legs are at the end of their swing phases and need to be grounded
	midswing is the pose in which all legs are at the highest point in their swing phases
	
	when a grounded leg enters the swing phases, its current pose is saved as the liftoff pose, then an animation is created using the liftoff pose, midswing pose, and touchdown pose.
	*/
	
	// Update transform
	if (state != Ant::State::DEAD)
	{
		transform.translation = getPosition();
		transform.rotation = getRotation();
			
		// Update model instance
		modelInstance.setTransform(transform);
	}
}

Vector3 Ant::forage(const Vector3& leftReceptor, const Vector3& rightReceptor)
{
	float leftSignal = 0.0f;
	float rightSignal = 0.0f;
	
	// Detect pheromones with left receptor
	std::list<Pheromone*> leftPheromones;
	colony->getPheromoneOctree()->query(AABB(leftReceptor, leftReceptor), &leftPheromones);
	for (Pheromone* pheromone: leftPheromones)
	{
		Vector3 difference = pheromone->getPosition() - rightReceptor;
		
		float distanceSquared = glm::dot(difference, difference);
		if (distanceSquared <= pheromone->getRadiusSquared())
		{
			// Calculate attenuated pheromone strength using inverse-square law
			float strength = pheromone->getStrength() / ((distanceSquared == 0.0f) ? 1.0f : distanceSquared);
			leftSignal += strength;
		}
	}
	
	// Detect pheromones with right receptor
	std::list<Pheromone*> rightPheromones;
	colony->getPheromoneOctree()->query(AABB(rightReceptor, rightReceptor), &rightPheromones);
	for (Pheromone* pheromone: rightPheromones)
	{
		Vector3 difference = pheromone->getPosition() - rightReceptor;
		
		float distanceSquared = glm::dot(difference, difference);
		if (distanceSquared <= pheromone->getRadiusSquared())
		{
			// Calculate attenuated pheromone strength using inverse-square law
			float strength = pheromone->getStrength() / ((distanceSquared == 0.0f) ? 1.0f : distanceSquared);
			rightSignal += strength;
		}
	}
	
	// Add noise
	const float maxNoise = 0.1f;
	leftSignal += frand(0.0f, maxNoise);
	rightSignal += frand(0.0f, maxNoise);
	
    if (leftSignal + rightSignal > 0.0f)
    {
		const float maxPheromoneTurningAngle = 0.1f;
		
		// Use Weber's law (Perna et al.) to calculate turning angle based on pheromone signals
		float turningAngle = maxPheromoneTurningAngle * ((leftSignal - rightSignal) / (leftSignal + rightSignal));
    }
	
	return Vector3(0.0f);
}

void Ant::setState(Ant::State state)
{
	this->state = state;
}

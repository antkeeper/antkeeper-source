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

#include "behavior-system.hpp"
#include <set>

BehaviorSystem::BehaviorSystem(ComponentManager* componentManager):
	System(componentManager),
	behaviorGroup(componentManager),
	antHillGroup(componentManager)
{
	behaviorGroup.addGroupObserver(this);
}

BehaviorSystem::~BehaviorSystem()
{}

void BehaviorSystem::update(float t, float dt)
{
	auto members = behaviorGroup.getMembers();
	
	for (const BehaviorGroup::Member* member: *members)
	{
		BehaviorComponent* behavior = std::get<0>(member->components);
		LeggedLocomotionComponent* leggedLocomotion = std::get<1>(member->components);
		SteeringComponent* steering = std::get<2>(member->components);
		TransformComponent* transform = std::get<3>(member->components);

		steering->maxSpeed = leggedLocomotion->speed;

		steering->behaviorCount = 2;
		steering->behaviors[0].priority = 2.0f;
		steering->behaviors[0].weight = 1.0f;
		steering->behaviors[0].function = std::bind(&BehaviorSystem::containment, this, member);

		steering->behaviors[1].priority = 1.0f;
		steering->behaviors[1].weight = 0.5f;
		steering->behaviors[1].function = std::bind(&BehaviorSystem::wander, this, dt, member);
	}
}

void BehaviorSystem::memberRegistered(const BehaviorGroup::Member* member)
{
	BehaviorComponent* behavior = std::get<0>(member->components);
	LeggedLocomotionComponent* leggedLocomotion = std::get<1>(member->components);

	behavior->wanderDirection = Vector3(0.0f);
	while (glm::length2(behavior->wanderDirection) == 0.0f)
	{
		behavior->wanderDirection = Vector3(frand(-1, 1), frand(-1, 1), frand(-1, 1));
	}

	behavior->wanderTriangle = leggedLocomotion->surface;
	behavior->wanderDirection = glm::normalize(behavior->wanderDirection);
	behavior->wanderCircleDistance = 3.0f;
	behavior->wanderCircleRadius = 2.0f;
	behavior->wanderRate = glm::radians(180.0f) * 5.0f;

	leggedLocomotion->speed = 2.0f;
}

void BehaviorSystem::memberUnregistered(const BehaviorGroup::Member* member)
{}


Vector3 BehaviorSystem::containment(const BehaviorGroup::Member* agent)
{
	LeggedLocomotionComponent* leggedLocomotion = std::get<1>(agent->components);
	TransformComponent* transform = std::get<3>(agent->components);

	float probeAngle = glm::radians(30.0f);
	float probeDistance = 5.0f;

	Vector3 direction = transform->transform.rotation * Vector3(0, 0, 1);

	TriangleMesh::Triangle* surface = leggedLocomotion->surface;

	Vector3 forward = transform->transform.rotation * Vector3(0, 0, 1);
	Vector3 up = surface->normal;
	Vector3 right = glm::normalize(glm::cross(forward, up));

	Vector3 force(0.0f);

	return force;
}

Vector3 BehaviorSystem::wander(float dt, const BehaviorGroup::Member* agent)
{
	BehaviorComponent* behavior = std::get<0>(agent->components);
	LeggedLocomotionComponent* leggedLocomotion = std::get<1>(agent->components);
	SteeringComponent* steering = std::get<2>(agent->components);
	TransformComponent* transform = std::get<3>(agent->components);

	// Reorientate wander direction
	if (behavior->wanderTriangle != leggedLocomotion->surface)
	{
		if (behavior->wanderTriangle)
		{
		behavior->wanderDirection = glm::normalize(glm::rotation(behavior->wanderTriangle->normal, leggedLocomotion->surface->normal) * behavior->wanderDirection);
		}
		behavior->wanderTriangle = leggedLocomotion->surface;
	}

	// Make wander direction coplanar with surface triangle
	TriangleMesh::Triangle* triangle = leggedLocomotion->surface;
	Vector3 triangleCenter = (triangle->edge->vertex->position + triangle->edge->next->vertex->position + triangle->edge->previous->vertex->position) * (1.0f / 3.0f);
	behavior->wanderDirection = glm::normalize(projectOnPlane(transform->transform.translation + behavior->wanderDirection, triangleCenter, triangle->normal) - transform->transform.translation);

	Vector3 forward = transform->transform.rotation * Vector3(0, 0, 1);
	Vector3 up = triangle->normal;

	// Calculate center of wander circle
	Vector3 wanderCircleCenter = forward * behavior->wanderCircleDistance;

	// Calculate wander force
	Vector3 wanderForce = wanderCircleCenter + behavior->wanderDirection * behavior->wanderCircleRadius;

	// Displace wander direction
	float displacementAngle = frand(-behavior->wanderRate, behavior->wanderRate) * 0.5f * dt;
	behavior->wanderDirection = glm::normalize(glm::angleAxis(displacementAngle, up) * behavior->wanderDirection);

	return wanderForce;
}

Vector3 BehaviorSystem::forage(const BehaviorGroup::Member* agent)
{
	return Vector3(0.0f);
}

Vector3 BehaviorSystem::homing(const BehaviorGroup::Member* agent)
{
	// Get ant position
	const Vector3& antPosition = std::get<3>(agent->components)->transform.translation;

	// Find nearest ant-hill
	bool found = false;
	float minDistanceSquared = 0.0f;
	Vector3 homingDirection(0.0f);

	auto antHills = antHillGroup.getMembers();
	for (const AntHillGroup::Member* antHill: *antHills)
	{
		// Get ant-hill position
		const Vector3& antHillPosition = std::get<1>(antHill->components)->transform.translation;

		// Determine distance to ant-hill
		Vector3 difference = antHillPosition - antPosition;
		float distanceSquared =  glm::length2(difference);

		if (!found || distanceSquared < minDistanceSquared)
		{
			minDistanceSquared = distanceSquared;
			homingDirection = difference;
			found = true;
		}
	}

	if (found)
	{
		homingDirection = glm::normalize(homingDirection);
	}
	
	return homingDirection;
}


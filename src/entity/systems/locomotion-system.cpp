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

#include "locomotion-system.hpp"
#include "../../triangle-mesh-operations.hpp"

LocomotionSystem::LocomotionSystem(ComponentManager* componentManager):
	System(componentManager),
	leggedLocomotionGroup(componentManager)
{
	leggedLocomotionGroup.addGroupObserver(this);
}

LocomotionSystem::~LocomotionSystem()
{}

void LocomotionSystem::update(float t, float dt)
{
	auto members = leggedLocomotionGroup.getMembers();
	
	// Perform legged locomotion
	for (const LeggedLocomotionGroup::Member* member: *members)
	{
		LeggedLocomotionComponent* leggedLocomotion = std::get<0>(member->components);
		SteeringComponent* steering = std::get<1>(member->components);
		TransformComponent* transform = std::get<2>(member->components);

		// Skip entities which are not on a surface
		if (!leggedLocomotion->surface)
		{
			continue;
		}

		// Determine target position
		Vector3 force = steering->force * dt;
		float speed = steering->speed * dt;

		if (speed == 0.0f)
		{
			continue;
		}

		// Calculate direction vector
		Vector3 direction = force * (1.0f / speed);

		std::vector<WrapOperationSegment> segments;
		float wrapDistance = wrap(leggedLocomotion->surface, transform->transform.translation, direction, speed, &segments);

		WrapOperationSegment segment = segments.back();

		Vector3 cartesianStart = cartesian(segment.startPosition,
			segment.triangle->edge->vertex->position,
			segment.triangle->edge->next->vertex->position,
			segment.triangle->edge->previous->vertex->position);
		Vector3 cartesianEnd = cartesian(segment.endPosition,
			segment.triangle->edge->vertex->position,
			segment.triangle->edge->next->vertex->position,
			segment.triangle->edge->previous->vertex->position);

		// Calculate wrap direction of final segment
		Vector3 segmentDirection(0.0f);
		if (cartesianStart != cartesianEnd)
		{
			segmentDirection = glm::normalize(cartesianEnd - cartesianStart);
		}

		// Determine angle between the triangles
		float angle = std::acos(glm::dot(leggedLocomotion->surface->normal, segment.triangle->normal));
		if (std::abs(angle) > glm::radians(35.0f))
		{
			// Transition
		}


		leggedLocomotion->surface = segment.triangle;
		leggedLocomotion->barycentricPosition = segment.endPosition;
		transform->transform.translation = cartesianEnd;
		if (cartesianStart != cartesianEnd)
		{
			transform->transform.rotation = lookRotation(segmentDirection, segment.triangle->normal);
		}
	}
}

void LocomotionSystem::memberRegistered(const LeggedLocomotionGroup::Member* member)
{}

void LocomotionSystem::memberUnregistered(const LeggedLocomotionGroup::Member* member)
{}



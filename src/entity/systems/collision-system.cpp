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

#include "collision-system.hpp"

CollisionSystem::CollisionSystem(ComponentManager* componentManager):
	System(componentManager),
	entityGroup(componentManager)
{}

CollisionSystem::~CollisionSystem()
{}

void CollisionSystem::update(float t, float dt)
{
	auto members = entityGroup.getMembers();
	
	// INEFFICIENT! Currently done twice (A vs B, then B vs A)
	// Also no octrees or other structure
	for (const CollisionEntityGroup::Member* memberA: *members)
	{
		CollisionComponent* collisionA = std::get<0>(memberA->components);
		TransformComponent* transformA = std::get<1>(memberA->components);

		// Clear previous collisions
		collisionA->collisions.clear();

		for (const CollisionEntityGroup::Member* memberB: *members)
		{
			if (memberA == memberB)
			{
				continue;
			}

			CollisionComponent* collisionB = std::get<0>(memberB->components);
			TransformComponent* transformB = std::get<1>(memberB->components);

			Vector3 difference = transformA->transform.translation - transformB->transform.translation;
			float distanceSquared = glm::length2(difference);
			float collisionRadius = collisionA->radius + collisionB->radius;

			if (distanceSquared <= collisionRadius * collisionRadius)
			{
				collisionA->collisions.push_back(memberB->entity);
			}
		}
	}
}


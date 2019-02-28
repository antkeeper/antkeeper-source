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

#include "steering-system.hpp"

SteeringSystem::SteeringSystem(ComponentManager* componentManager):
	System(componentManager),
	boids(componentManager)
{
	boids.addGroupObserver(this);
}

SteeringSystem::~SteeringSystem()
{}

void SteeringSystem::update(float t, float dt)
{
	auto members = boids.getMembers();

	for (const SteeringGroup::Member* member: *members)
	{
		SteeringComponent* steering = std::get<0>(member->components);

		// Initialize summed steering force
		steering->force = Vector3(0.0f);
		steering->speed = 0.0f;
		float speedSquared = 0.0f;
		float maxSpeedSquared = steering->maxSpeed * steering->maxSpeed;
		bool truncated = false;

		if (steering->behaviorCount > 0)
		{
			// Sort steering beaviors by priority
			std::sort(steering->behaviors, steering->behaviors + steering->behaviorCount,
				[](const SteeringBehavior& a, const SteeringBehavior& b) -> bool
				{
					return (a.priority >= b.priority);
				}
			);
		}

		// Evaluate steering forces in order
		for (std::size_t i = 0; i < steering->behaviorCount; ++i)
		{
			const SteeringBehavior& behavior = steering->behaviors[i];

			// Skip zero-weighted steering behaviors
			if (behavior.weight == 0.0f)
			{
				continue;
			}

			// Add weighted steering behavior force
			steering->force += behavior.function() * behavior.weight;

			// Limit speed
			speedSquared = glm::length2(steering->force);
			if (speedSquared >= maxSpeedSquared)
			{
				steering->force *= (1.0f / std::sqrt(speedSquared)) * steering->maxSpeed;
				steering->speed = steering->maxSpeed;
				truncated = true;
				break;
			}
		}

		if (!truncated)
		{
			steering->speed = std::sqrt(speedSquared);
		}
	}
}

void SteeringSystem::memberRegistered(const SteeringGroup::Member* member)
{}

void SteeringSystem::memberUnregistered(const SteeringGroup::Member* member)
{}



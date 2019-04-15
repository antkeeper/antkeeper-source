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

#include "constraint-system.hpp"

ConstraintSystem::ConstraintSystem(ComponentManager* componentManager):
	System(componentManager),
	orbitConstraintGroup(componentManager)
{}

ConstraintSystem::~ConstraintSystem()
{}

void ConstraintSystem::update(float t, float dt)
{
	// Solve orbit constraints
	for (const OrbitConstraintGroup::Member* member: *orbitConstraintGroup.getMembers())
	{
		OrbitConstraintComponent* constraint = std::get<0>(member->components);
		TransformComponent* transform = std::get<1>(member->components);

		// Get target transform component
		TransformComponent* target = componentManager->getComponent<TransformComponent>(constraint->target);
		if (!target)
		{
			continue;
		}

		// Calculate rotation quaternion
		Quaternion azimuthRotation = glm::angleAxis(constraint->azimuth, Vector3(0.0f, 1.0f, 0.0f));
		Quaternion elevationRotation = glm::angleAxis(constraint->elevation, Vector3(-1.0f, 0.0f, 0.0f));
		Quaternion rotation = azimuthRotation * elevationRotation;

		// Calculate translation vector
		Vector3 translation = target->transform.translation + rotation * Vector3(0, 0, constraint->distance);

		// Update transform
		transform->transform.translation = translation;
		transform->transform.rotation = rotation;
	}
}


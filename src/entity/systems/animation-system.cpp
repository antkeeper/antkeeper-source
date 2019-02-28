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

#include "animation-system.hpp"

AnimationSystem::AnimationSystem(ComponentManager* componentManager):
	System(componentManager),
	animationGroup(componentManager)
{}

AnimationSystem::~AnimationSystem()
{}

void AnimationSystem::update(float t, float dt)
{
	auto members = animationGroup.getMembers();

	for (const AnimationEntityGroup::Member* member: *members)
	{
		AnimationComponent* animationComponent = std::get<0>(member->components);
		ModelComponent* modelComponent = std::get<1>(member->components);

		Pose* pose = modelComponent->model.getPose();

		for (const std::pair<AnimationClip<Transform>, float>& clipWeightPair: animationComponent->animations)
		{
			const AnimationClip<Transform>& clip = clipWeightPair.first;
			float weight = clipWeightPair.second;
		}
	}
}


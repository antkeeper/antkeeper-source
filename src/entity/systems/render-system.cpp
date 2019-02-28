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

#include "render-system.hpp"

RenderSystem::RenderSystem(ComponentManager* componentManager, SceneLayer* scene):
	System(componentManager),
	modelEntityGroup(componentManager),
	scene(scene)
{
	modelEntityGroup.addGroupObserver(this);
}

RenderSystem::~RenderSystem()
{}

void RenderSystem::update(float t, float dt)
{
	auto members = modelEntityGroup.getMembers();
	for (const ModelEntityGroup::Member* member: *members)
	{
		ModelComponent* model = std::get<0>(member->components);
		TransformComponent* transform = std::get<1>(member->components);

		model->model.setTransform(transform->transform);
	}
}

void RenderSystem::memberRegistered(const ModelEntityGroup::Member* member)
{
	ModelComponent* model = std::get<0>(member->components);
	scene->addObject(&model->model);
}

void RenderSystem::memberUnregistered(const ModelEntityGroup::Member* member)
{
	ModelComponent* model = std::get<0>(member->components);
	scene->removeObject(&model->model);
}


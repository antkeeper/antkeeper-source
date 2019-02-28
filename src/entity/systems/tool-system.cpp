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

#include "tool-system.hpp"

ToolSystem::ToolSystem(ComponentManager* componentManager):
	System(componentManager),
	tools(componentManager),
	picked(false)
{
	tools.addGroupObserver(this);
}

ToolSystem::~ToolSystem()
{}

void ToolSystem::update(float t, float dt)
{
	pick();

	auto members = tools.getMembers();	
	for (const ToolGroup::Member* member: *members)
	{
		ModelComponent* model = std::get<0>(member->components);
		ToolComponent* tool = std::get<1>(member->components);
		TransformComponent* transform = std::get<2>(member->components);
		
		model->model.setActive(tool->active);

		if (picked)
		{
			transform->transform.translation = mouseWorldPosition;
		}
	}

	picked = false;
}

void ToolSystem::setPickingCamera(const Camera* camera)
{
	pickingCamera = camera;
}

void ToolSystem::setPickingViewport(const Vector4& viewport)
{
	pickingViewport = viewport;
}

void ToolSystem::pick()
{
	Vector3 mouseNear = pickingCamera->unproject(Vector3(mouseScreenPosition, 0.0f), pickingViewport);
	Vector3 mouseFar = pickingCamera->unproject(Vector3(mouseScreenPosition, 1.0f), pickingViewport);

	Ray pickingRay;
	pickingRay.origin = mouseNear;
	pickingRay.direction = glm::normalize(mouseFar - mouseNear);
	Plane pickingPlane(Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f));

	auto pickingIntersection = pickingRay.intersects(pickingPlane);
	picked = std::get<0>(pickingIntersection);
	if (picked)
	{
		mouseWorldPosition = pickingRay.extrapolate(std::get<1>(pickingIntersection));
	}
}

void ToolSystem::memberRegistered(const ToolGroup::Member* member)
{
	ToolComponent* tool = std::get<1>(member->components);
	tool->active = false;
}

void ToolSystem::memberUnregistered(const ToolGroup::Member* member)
{}

void ToolSystem::handleEvent(const MouseMovedEvent& event)
{
	mouseScreenPosition = Vector2(event.x, pickingViewport[3] - event.y);
}


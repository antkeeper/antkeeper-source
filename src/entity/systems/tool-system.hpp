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

#ifndef TOOL_SYSTEM_HPP
#define TOOL_SYSTEM_HPP

#include "../entity-group.hpp"
#include "../components/model-component.hpp"
#include "../components/tool-component.hpp"
#include "../components/transform-component.hpp"
#include "../system.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

typedef EntityGroup<ModelComponent, ToolComponent, TransformComponent> ToolGroup;

/**
 * Abstract base class for entity systems.
 */
class ToolSystem:
	public System,
	public ToolGroup::Observer,
	public EventHandler<MouseMovedEvent>
{
public:
	ToolSystem(ComponentManager* componentManager);
	virtual ~ToolSystem();
	
	virtual void update(float t, float dt);

	void setPickingCamera(const Camera* camera);
	void setPickingViewport(const Vector4& viewport);

private:
	void pick();
	virtual void memberRegistered(const ToolGroup::Member* member);
	virtual void memberUnregistered(const ToolGroup::Member* member);
	virtual void handleEvent(const MouseMovedEvent& event);

	Vector2 mouseScreenPosition;
	Vector3 mouseWorldPosition;
	const Camera* pickingCamera;
	Vector4 pickingViewport;
	bool picked;
	ToolGroup tools;
};

#endif // TOOL_SYSTEM_HPP


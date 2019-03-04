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

#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "../components/camera-component.hpp"
#include "../components/model-component.hpp"
#include "../components/transform-component.hpp"
#include "../entity-group.hpp"
#include "../system.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

typedef EntityGroup<CameraComponent, TransformComponent> CameraGroup;
typedef EntityGroup<ModelComponent, TransformComponent> ModelGroup;

/**
 * Abstract base class for entity systems.
 */
class RenderSystem:
	public System,
	public CameraGroup::Observer,
	public ModelGroup::Observer
{
public:
	RenderSystem(ComponentManager* componentManager, Scene* scene);
	virtual ~RenderSystem();
	
	virtual void update(float t, float dt);

private:
	CameraGroup cameraGroup;
	ModelGroup modelGroup;
	Scene* scene;

	virtual void memberRegistered(const CameraGroup::Member* member);
	virtual void memberUnregistered(const CameraGroup::Member* member);
	virtual void memberRegistered(const ModelGroup::Member* member);
	virtual void memberUnregistered(const ModelGroup::Member* member);
};

#endif // RENDER_SYSTEM_HPP


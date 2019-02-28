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

#ifndef BEHAVIOR_SYSTEM_HPP
#define BEHAVIOR_SYSTEM_HPP

#include "../entity-group.hpp"
#include "../components/ant-hill-component.hpp"
#include "../components/behavior-component.hpp"
#include "../components/legged-locomotion-component.hpp"
#include "../components/steering-component.hpp"
#include "../components/transform-component.hpp"
#include "../system.hpp"

#include <emergent/emergent.hpp>
using namespace Emergent;

typedef EntityGroup<BehaviorComponent, LeggedLocomotionComponent, SteeringComponent, TransformComponent> BehaviorGroup;
typedef EntityGroup<AntHillComponent, TransformComponent> AntHillGroup;

class BehaviorSystem: public
	System,
	BehaviorGroup::Observer
{
public:
	BehaviorSystem(ComponentManager* componentManager);
	virtual ~BehaviorSystem();
	
	virtual void update(float t, float dt);

private:
	BehaviorGroup behaviorGroup;
	AntHillGroup antHillGroup;

	virtual void memberRegistered(const BehaviorGroup::Member* member);
	virtual void memberUnregistered(const BehaviorGroup::Member* member);

	Vector3 containment(const BehaviorGroup::Member* agent);
	Vector3 wander(float dt, const BehaviorGroup::Member* agent);
	Vector3 forage(const BehaviorGroup::Member* agent);
	Vector3 homing(const BehaviorGroup::Member* agent);
};

#endif // BEHAVIOR_SYSTEM_HPP


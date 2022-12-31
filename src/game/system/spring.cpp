/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "game/system/spring.hpp"
#include "game/component/spring.hpp"
#include "entity/id.hpp"

namespace game {
namespace system {

spring::spring(entity::registry& registry):
	updatable(registry)
{}

spring::~spring()
{}

void spring::update(double t, double dt)
{
	const float dtf = static_cast<float>(dt);
	
	registry.view<component::spring1>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float, float>(component.spring, dtf);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
	
	registry.view<component::spring2>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float2, float>(component.spring, dtf);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
	
	registry.view<component::spring3>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float3, float>(component.spring, dtf);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
	
	registry.view<component::spring4>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float4, float>(component.spring, dtf);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
}

} // namespace system
} // namespace game

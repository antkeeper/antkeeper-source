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

#include "game/systems/spring-system.hpp"
#include "game/components/spring-component.hpp"
#include <engine/entity/id.hpp>


spring_system::spring_system(entity::registry& registry):
	updatable_system(registry)
{}

spring_system::~spring_system()
{}

void spring_system::update(float t, float dt)
{
	registry.view<spring1_component>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float, float>(component.spring, dt);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
	
	registry.view<spring2_component>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float2, float>(component.spring, dt);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
	
	registry.view<spring3_component>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float3, float>(component.spring, dt);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
	
	registry.view<spring4_component>().each
	(
		[&](entity::id spring_eid, auto& component)
		{
			solve_numeric_spring<float4, float>(component.spring, dt);
			if (component.callback)
				component.callback(component.spring.x0);
		}
	);
}


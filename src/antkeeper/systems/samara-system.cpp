/*
 * Copyright (C) 2020  Christopher J. Howard
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

#include "samara-system.hpp"
#include "entity/components/transform-component.hpp"
#include "entity/components/samara-component.hpp"
#include "math.hpp"

using namespace vmq::operators;
using namespace ecs;

samara_system::samara_system(entt::registry& registry):
	entity_system(registry)
{}

void samara_system::update(double t, double dt)
{
	registry.view<samara_component, transform_component>().each(
		[&](auto entity, auto& samara, auto& transform)
		{
			samara.angle += samara.chirality * vmq::radians(360.0f * 6.0f) * dt;

			transform.transform.translation += samara.direction * 20.0f * (float)dt;
			transform.transform.rotation =
				vmq::angle_axis(samara.angle, float3{0, 1, 0}) *
				vmq::angle_axis(vmq::radians(20.0f), float3{1, 0, 0}) *
				((samara.chirality < 0.0f) ? vmq::angle_axis(vmq::radians(180.0f), float3{0, 0, -1}) : vmq::quaternion<float>{1, 0, 0, 0});

			if (transform.transform.translation.y < 0.0f)
			{
				const float zone = 200.0f;
				transform.transform.translation.x = frand(-zone, zone);
				transform.transform.translation.y = frand(100.0f, 150.0f);
				transform.transform.translation.z = frand(-zone, zone);
				transform.warp = true;

				samara.chirality = (frand(0, 1) < 0.5f) ? -1.0f : 1.0f;
			}
		});
}


/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "nest-system.hpp"
#include "nest.hpp"
#include "math/math.hpp"

using namespace ecs;

nest_system::nest_system(entt::registry& registry, ::resource_manager* resource_manager):
	entity_system(registry),
	resource_manager(resource_manager)
{
	registry.on_construct<nest_component>().connect<&nest_system::on_nest_construct>(this);
	registry.on_destroy<nest_component>().connect<&nest_system::on_nest_destroy>(this);
}

nest_system::~nest_system()
{}

void nest_system::update(double t, double dt)
{}

void nest_system::on_nest_construct(entt::registry& registry, entt::entity entity, nest_component& component)
{
	// Allocate a nest
	nest* nest = new ::nest();

	// Setup initial nest parameters
	nest->set_tunnel_radius(1.15f);
	nest::shaft* central_shaft = nest->get_central_shaft();
	central_shaft->chirality = -1.0f;
	central_shaft->rotation = math::radians(0.0f);
	central_shaft->depth = {0.0f, 100.0f};
	central_shaft->current_depth = 0.0f;
	central_shaft->radius = {0.0f, 5.0f};
	central_shaft->pitch = {4.0f, 8.0f};
	central_shaft->translation = {{{0.0f, 0.0f}, {20.0f, 11.0f}}};
	for (std::size_t i = 0; i < 4; ++i)
	{
		nest::chamber chamber;
		chamber.shaft = central_shaft;
		chamber.depth = (i + 1) * 23.0f;
		chamber.rotation = math::radians(0.0f);
		chamber.inner_radius = 4.0f;
		chamber.outer_radius = 10.0f;
		central_shaft->chambers.push_back(chamber);
	}
}

void nest_system::on_nest_destroy(entt::registry& registry, entt::entity entity)
{}


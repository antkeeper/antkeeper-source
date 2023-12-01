// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/behavior-system.hpp"
#include "game/components/behavior-component.hpp"
#include <engine/entity/id.hpp>


behavior_system::behavior_system(entity::registry& registry):
	updatable_system(registry)
{}

void behavior_system::update(float t, float dt)
{
	// registry.view<behavior_component>().each(
		// [&](entity::id entity_id, auto& behavior)
		// {
		// });
}


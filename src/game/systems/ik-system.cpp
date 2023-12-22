// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/ik-system.hpp"
#include "game/components/ik-component.hpp"
#include <engine/entity/id.hpp>
#include <algorithm>
#include <execution>

ik_system::ik_system(entity::registry& registry):
	updatable_system(registry)
{}

void ik_system::update([[maybe_unused]] float t, [[maybe_unused]] float dt)
{
	auto view = m_registry.view<ik_component>();
	std::for_each
	(
		std::execution::par_unseq,
		view.begin(),
		view.end(),
		[&](auto entity_id)
		{
			const auto& component = view.get<ik_component>(entity_id);
			
			component.rig->solve();
		}
	);
}

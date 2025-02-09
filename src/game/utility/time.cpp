// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/utility/time.hpp"
#include "game/components/time-component.hpp"

void set_time_scale(entity::registry& registry, float time_scale)
{
	if (auto view = registry.view<time_component>(); !view.empty())
	{
		view.get<time_component>(view.front()).time_scale = time_scale;
	}
	else
	{
		auto entity_id = registry.create();
		registry.emplace<time_component>(entity_id, time_scale);
	}
}

float get_time_scale(const entity::registry& registry)
{
	if (auto view = registry.view<const time_component>(); !view.empty())
	{
		return view.get<const time_component>(view.front()).time_scale;
	}

	return 1.0f;
}

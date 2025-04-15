// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/systems/frame-interpolation-system.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-object-component.hpp"

void frame_interpolation_system::variable_update(entity::registry& registry, float, float, float alpha)
{
	registry.view<scene_object_component, const rigid_body_component>().each
	(
		[alpha](auto, auto& scene, const auto& rigid_body)
		{
			scene.object->set_transform(rigid_body.body->interpolate(alpha));
		}
	);
}

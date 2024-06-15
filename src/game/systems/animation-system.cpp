// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/animation-system.hpp"
#include "game/components/pose-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/animation/bone.hpp>
#include <engine/scene/skeletal-mesh.hpp>
#include <engine/math/functions.hpp>
#include <algorithm>
#include <execution>

animation_system::animation_system(entity::registry& registry):
	updatable_system(registry)
{}

void animation_system::update([[maybe_unused]] float t, [[maybe_unused]] float dt)
{
	
}

void animation_system::interpolate(float alpha)
{
	auto pose_group = m_registry.group<pose_component>(entt::get<scene_component>);
	std::for_each
	(
		std::execution::par_unseq,
		pose_group.begin(),
		pose_group.end(),
		[&](auto entity_id)
		{
			auto& pose = pose_group.get<pose_component>(entity_id);
			auto& scene = pose_group.get<scene_component>(entity_id);
			
			auto& skeletal_mesh = static_cast<scene::skeletal_mesh&>(*scene.object);
			for (std::size_t i = 0; i < skeletal_mesh.get_skeleton()->bones().size(); ++i)
			{
				const auto& previous_transform = pose.previous_pose.get_relative_transform(i);
				const auto& current_transform = pose.current_pose.get_relative_transform(i);
				
				// Interpolate bone pose between previous and current states
				math::transform<float> interpolated_transform;
				interpolated_transform.translation = math::lerp(previous_transform.translation, current_transform.translation, alpha);
				interpolated_transform.rotation = math::nlerp(previous_transform.rotation, current_transform.rotation, alpha);
				interpolated_transform.scale = math::lerp(previous_transform.scale, current_transform.scale, alpha);
				
				// Update skeletal mesh bone pose with interpolated transform
				skeletal_mesh.get_pose().set_relative_transform(i, interpolated_transform);
			}
		}
	);
}


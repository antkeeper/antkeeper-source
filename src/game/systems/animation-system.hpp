// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP
#define ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
#include "game/systems/variable-update-system.hpp"
#include <engine/entity/id.hpp>
#include <memory>
#include <vector>

using namespace engine;

class animation_player;

class animation_system:
	public fixed_update_system,
	public variable_update_system
{
public:
	explicit animation_system(entity::registry& registry);
	~animation_system() override;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	void variable_update(entity::registry& registry, float t, float dt, float alpha) override;

private:
	void on_animation_construct(entity::registry& registry, entity::id entity);

	entity::registry& m_registry;
	float m_previous_render_time{};
	float m_render_time{};
};

#endif // ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP

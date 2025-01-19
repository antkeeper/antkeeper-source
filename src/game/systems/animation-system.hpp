// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP
#define ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include <memory>
#include <vector>

class animation_player;

class animation_system:
	public updatable_system
{
public:
	explicit animation_system(entity::registry& registry);
	~animation_system() override;
	void update(float t, float dt) override;
	void interpolate(float alpha);

private:
	void on_animation_construct(entity::registry& registry, entity::id entity);

	float m_previous_update_time{};
	float m_update_time{};
	float m_fixed_timestep{};

	float m_previous_render_time{};
	float m_render_time{};
};

#endif // ANTKEEPER_GAME_ANIMATION_SYSTEM_HPP

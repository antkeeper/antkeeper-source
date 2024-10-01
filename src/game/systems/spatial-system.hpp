// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SPATIAL_SYSTEM_HPP
#define ANTKEEPER_GAME_SPATIAL_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <entt/entt.hpp>

class spatial_system:
	public updatable_system
{
public:
	explicit spatial_system(entity::registry& registry);
	~spatial_system() override = default;
	
	virtual void update(float t, float dt);
	
private:
	/// Observes entities with updated, unconstrained transforms.
	entt::observer m_updated_unconstrained_transforms;
};

#endif // ANTKEEPER_GAME_SPATIAL_SYSTEM_HPP

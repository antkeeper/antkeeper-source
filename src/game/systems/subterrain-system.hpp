// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_SUBTERRAIN_SYSTEM_HPP
#define ANTKEEPER_GAME_SUBTERRAIN_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/resources/resource-manager.hpp>

class subterrain_system: public updatable_system
{
public:
	subterrain_system(entity::registry& registry, ::resource_manager* resource_manager);
	~subterrain_system() override;
	virtual void update(float t, float dt);

private:
};

#endif // ANTKEEPER_GAME_SUBTERRAIN_SYSTEM_HPP

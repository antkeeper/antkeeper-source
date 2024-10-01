// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/subterrain-system.hpp"

subterrain_system::subterrain_system(entity::registry& registry, [[maybe_unused]] ::resource_manager* resource_manager):
	updatable_system(registry)
{}

subterrain_system::~subterrain_system()
{}

void subterrain_system::update([[maybe_unused]] float t, [[maybe_unused]] float dt)
{}

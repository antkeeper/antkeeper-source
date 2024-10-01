// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/morphogenesis-system.hpp"
#include <engine/entity/id.hpp>


morphogenesis_system::morphogenesis_system(entity::registry& registry):
	updatable_system(registry)
{}

void morphogenesis_system::update([[maybe_unused]] float t, [[maybe_unused]] float dt)
{}


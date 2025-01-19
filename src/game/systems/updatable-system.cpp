// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/updatable-system.hpp"


updatable_system::updatable_system(entity::registry& registry):
	m_registry(registry)
{}


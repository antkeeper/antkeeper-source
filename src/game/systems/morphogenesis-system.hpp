// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_MORPHOGENESIS_SYSTEM_HPP
#define ANTKEEPER_GAME_MORPHOGENESIS_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"

/// Generates 3D models from genomes.
class morphogenesis_system:
	public updatable_system
{
public:
	explicit morphogenesis_system(entity::registry& registry);
	~morphogenesis_system() override = default;
	
	virtual void update(float t, float dt);
	
private:
};


#endif // ANTKEEPER_GAME_MORPHOGENESIS_SYSTEM_HPP

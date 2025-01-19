// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_BLACKBODY_SYSTEM_HPP
#define ANTKEEPER_GAME_BLACKBODY_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include "game/components/blackbody-component.hpp"
#include <vector>

/// Calculates the color and luminance of blackbody radiators.
class blackbody_system:
	public updatable_system
{
public:
	explicit blackbody_system(entity::registry& registry);
	~blackbody_system() override;
	
	void update(float t, float dt) override;
	
private:
	void update_blackbody(entity::id entity_id);
	
	void on_blackbody_construct(entity::registry& registry, entity::id entity_id);
	void on_blackbody_update(entity::registry& registry, entity::id entity_id);
	
	std::vector<double> m_visible_wavelengths_nm;
};


#endif // ANTKEEPER_GAME_BLACKBODY_SYSTEM_HPP

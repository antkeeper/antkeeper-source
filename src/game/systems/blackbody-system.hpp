/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_BLACKBODY_SYSTEM_HPP
#define ANTKEEPER_GAME_BLACKBODY_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include "game/components/blackbody-component.hpp"
#include <vector>

/**
 * Calculates the color and luminance of blackbody radiators.
 */
class blackbody_system:
	public updatable_system
{
public:
	explicit blackbody_system(entity::registry& registry);
	~blackbody_system();
	
	void update(float t, float dt) override;
	
	/**
	 * Sets the blackbody illuminant.
	 *
	 * @param illuminant CIE chromaticity coordinates of an illuminant.
	 */
	void set_illuminant(const math::vec2<double>& illuminant);
	
private:
	void update_blackbody(entity::id entity_id);
	
	void on_blackbody_construct(entity::registry& registry, entity::id entity_id);
	void on_blackbody_update(entity::registry& registry, entity::id entity_id);
	
	std::vector<double> m_visible_wavelengths_nm;
	math::vec2<double> m_illuminant;
	math::mat3<double> m_xyz_to_rgb;
};


#endif // ANTKEEPER_GAME_BLACKBODY_SYSTEM_HPP

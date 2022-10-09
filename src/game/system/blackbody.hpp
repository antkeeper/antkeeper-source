/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_SYSTEM_BLACKBODY_HPP
#define ANTKEEPER_GAME_SYSTEM_BLACKBODY_HPP

#include "game/system/updatable.hpp"
#include "entity/id.hpp"
#include "utility/fundamental-types.hpp"
#include "game/component/blackbody.hpp"
#include "game/component/celestial-body.hpp"
#include <vector>

namespace game {
namespace system {

/**
 * Calculates the RGB luminous intensity of blackbody radiators.
 */
class blackbody:
	public updatable
{
public:
	blackbody(entity::registry& registry);
	~blackbody();
	
	virtual void update(double t, double dt);
	
	/**
	 * Sets the blackbody illuminant.
	 *
	 * @param illuminant CIE chromaticity coordinates of an illuminant.
	 */
	void set_illuminant(const math::vector2<double>& illuminant);
	
private:
	void update_luminance(entity::id entity_id);
	
	void on_blackbody_construct(entity::registry& registry, entity::id entity_id);
	void on_blackbody_update(entity::registry& registry, entity::id entity_id);
	
	void on_celestial_body_construct(entity::registry& registry, entity::id entity_id);
	void on_celestial_body_update(entity::registry& registry, entity::id entity_id);
	
	math::vector2<double> illuminant;
	std::vector<double> visible_wavelengths_nm;
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_BLACKBODY_HPP

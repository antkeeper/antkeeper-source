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

#ifndef ANTKEEPER_ENTITY_SYSTEM_BLACKBODY_HPP
#define ANTKEEPER_ENTITY_SYSTEM_BLACKBODY_HPP

#include "entity/systems/updatable.hpp"
#include "entity/id.hpp"
#include "utility/fundamental-types.hpp"
#include "entity/components/blackbody.hpp"
#include "entity/components/celestial-body.hpp"
#include <vector>

namespace entity {
namespace system {

/**
 * Calculates the RGB luminous intensity of blackbody radiators.
 */
class blackbody:
	public updatable
{
public:
	blackbody(entity::registry& registry);
	
	virtual void update(double t, double dt);
	
	/**
	 * Sets the wavelengths of red, green, and blue light.
	 *
	 * @param wavelengths Vector containing the wavelengths of red (x), green (y), and blue (z) light, in nanometers.
	 */
	void set_rgb_wavelengths(const double3& wavelengths);
	
private:
	void update_luminance(entity::id entity_id);
	
	void on_blackbody_construct(entity::registry& registry, entity::id entity_id, entity::component::blackbody& blackbody);
	void on_blackbody_replace(entity::registry& registry, entity::id entity_id, entity::component::blackbody& blackbody);
	
	void on_celestial_body_construct(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body);
	void on_celestial_body_replace(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body);
	
	double3 rgb_wavelengths_nm;
	double3 rgb_wavelengths_m;
	std::vector<double> visible_wavelengths_nm;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_BLACKBODY_HPP

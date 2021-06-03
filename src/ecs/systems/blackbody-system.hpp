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

#ifndef ANTKEEPER_ECS_BLACKBODY_SYSTEM_HPP
#define ANTKEEPER_ECS_BLACKBODY_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/entity.hpp"
#include "utility/fundamental-types.hpp"
#include "ecs/components/blackbody-component.hpp"
#include "ecs/components/celestial-body-component.hpp"
#include <vector>

namespace ecs {

/**
 * Calculates the RGB luminous intensity of blackbody radiators.
 */
class blackbody_system:
	public entity_system
{
public:
	blackbody_system(ecs::registry& registry);
	
	virtual void update(double t, double dt);
	
private:
	void update_luminous_intensity(ecs::entity entity);
	
	void on_blackbody_construct(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody);
	void on_blackbody_replace(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody);
	
	void on_celestial_body_construct(ecs::registry& registry, ecs::entity entity, ecs::celestial_body_component& celestial_body);
	void on_celestial_body_replace(ecs::registry& registry, ecs::entity entity, ecs::celestial_body_component& celestial_body);
	
	double3 rgb_wavelengths_nm;
	double3 rgb_wavelengths_m;
	std::vector<double> visible_wavelengths_nm;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_BLACKBODY_SYSTEM_HPP

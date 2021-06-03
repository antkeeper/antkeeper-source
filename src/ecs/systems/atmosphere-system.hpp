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

#ifndef ANTKEEPER_ECS_ATMOSPHERE_SYSTEM_HPP
#define ANTKEEPER_ECS_ATMOSPHERE_SYSTEM_HPP

#include "entity-system.hpp"
#include "ecs/entity.hpp"
#include "utility/fundamental-types.hpp"
#include "ecs/components/atmosphere-component.hpp"

namespace ecs {

/**
 * Updates variables related to atmospheric scattering.
 */
class atmosphere_system:
	public entity_system
{
public:
	atmosphere_system(ecs::registry& registry);
	
	virtual void update(double t, double dt);
	
private:
	void update_coefficients(ecs::entity entity);
	
	void on_atmosphere_construct(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere);
	void on_atmosphere_replace(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere);
	
	double3 rgb_wavelengths_nm;
	double3 rgb_wavelengths_m;
};

} // namespace ecs

#endif // ANTKEEPER_ECS_ATMOSPHERE_SYSTEM_HPP

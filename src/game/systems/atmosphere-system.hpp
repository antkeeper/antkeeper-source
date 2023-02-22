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

#ifndef ANTKEEPER_GAME_ATMOSPHERE_SYSTEM_HPP
#define ANTKEEPER_GAME_ATMOSPHERE_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <engine/entity/id.hpp>
#include <engine/utility/fundamental-types.hpp>
#include "game/components/atmosphere-component.hpp"
#include <engine/render/passes/sky-pass.hpp>


/**
 * Updates variables related to atmospheric scattering.
 */
class atmosphere_system:
	public updatable_system
{
public:
	atmosphere_system(entity::registry& registry);
	~atmosphere_system();
	
	virtual void update(double t, double dt);
	
	/**
	 * Sets the wavelengths of red, green, and blue light.
	 *
	 * @param wavelengths Vector containing the wavelengths of red (x), green (y), and blue (z) light, in meters.
	 */
	void set_rgb_wavelengths(const double3& wavelengths);
	
	void set_sky_pass(::render::sky_pass* pass);
	
	/**
	 * Sets the entity ID of the active atmosphere.
	 *
	 * @param entity_id Entity ID of the active atmosphere.
	 */
	void set_active_atmosphere(entity::id entity_id);
	
private:
	void update_atmosphere(entity::id entity_id);
	void update_sky_pass();
	
	void on_atmosphere_construct(entity::registry& registry, entity::id entity_id);
	void on_atmosphere_update(entity::registry& registry, entity::id entity_id);
	void on_atmosphere_destroy(entity::registry& registry, entity::id entity_id);
	
	entity::id active_atmosphere_eid;
	double3 rgb_wavelengths;
	double3 rgb_ozone_cross_sections;
	::atmosphere_component* atmosphere_component;
	::render::sky_pass* sky_pass;
};


#endif // ANTKEEPER_GAME_ATMOSPHERE_SYSTEM_HPP

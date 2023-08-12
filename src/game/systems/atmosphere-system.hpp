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

#include <engine/entity/id.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/passes/sky-pass.hpp>
#include "game/components/atmosphere-component.hpp"
#include "game/systems/updatable-system.hpp"

/**
 * Updates variables related to atmospheric scattering.
 */
class atmosphere_system:
	public updatable_system
{
public:
	explicit atmosphere_system(entity::registry& registry);
	~atmosphere_system();
	
	virtual void update(float t, float dt);
	
	/**
	 * Sets the wavelengths of red, green, and blue light.
	 *
	 * @param wavelengths Vector containing the wavelengths of red (x), green (y), and blue (z) light, in nanometers.
	 */
	void set_rgb_wavelengths(const math::dvec3& wavelengths);
	
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
	
	entity::id m_active_atmosphere_eid{entt::null};
	math::dvec3 m_rgb_wavelengths_nm{};
	math::dvec3 m_rgb_wavelengths_m{};
	math::dvec3 m_rgb_ozone_cross_sections{};
	::render::sky_pass* m_sky_pass{};
};

#endif // ANTKEEPER_GAME_ATMOSPHERE_SYSTEM_HPP

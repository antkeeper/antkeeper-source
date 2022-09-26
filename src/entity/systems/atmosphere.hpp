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

#ifndef ANTKEEPER_ENTITY_SYSTEM_ATMOSPHERE_HPP
#define ANTKEEPER_ENTITY_SYSTEM_ATMOSPHERE_HPP

#include "entity/systems/updatable.hpp"
#include "entity/id.hpp"
#include "utility/fundamental-types.hpp"
#include "entity/components/atmosphere.hpp"
#include "render/passes/sky-pass.hpp"

namespace entity {
namespace system {

/**
 * Updates variables related to atmospheric scattering.
 */
class atmosphere:
	public updatable
{
public:
	atmosphere(entity::registry& registry);
	
	virtual void update(double t, double dt);
	
	/**
	 * Sets the wavelengths of red, green, and blue light.
	 *
	 * @param wavelengths Vector containing the wavelengths of red (x), green (y), and blue (z) light, in meters.
	 */
	void set_rgb_wavelengths(const double3& wavelengths);
	
	/**
	 * Sets ozone cross sections for red, green, and blue wavelengths.
	 *
	 * @param cross_sections Vector containing the ozone cross sections of red (x), green (y), and blue (z) light, in m-2/molecule.
	 */
	void set_rgb_ozone_cross_sections(const double3& cross_sections);
	
	void set_sky_pass(::render::sky_pass* pass);
	
private:
	void atmosphere_modified();
	void update_sky_pass();
	
	void on_atmosphere_construct(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& component);
	void on_atmosphere_replace(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& component);
	void on_atmosphere_destroy(entity::registry& registry, entity::id entity_id);
	
	double3 rgb_wavelengths;
	double3 rgb_ozone_cross_sections;
	entity::component::atmosphere* atmosphere_component;
	::render::sky_pass* sky_pass;
};

} // namespace system
} // namespace entity

#endif // ANTKEEPER_ENTITY_SYSTEM_ATMOSPHERE_HPP

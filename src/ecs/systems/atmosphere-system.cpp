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

#include "ecs/systems/atmosphere-system.hpp"
#include "physics/atmosphere.hpp"

namespace ecs {

atmosphere_system::atmosphere_system(ecs::registry& registry):
	entity_system(registry),
	rgb_wavelengths_nm{0, 0, 0},
	rgb_wavelengths_m{0, 0, 0}
{
	registry.on_construct<ecs::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_construct>(this);
	registry.on_replace<ecs::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_replace>(this);
}

void atmosphere_system::update(double t, double dt)
{}

void atmosphere_system::set_rgb_wavelengths(const double3& wavelengths)
{
	rgb_wavelengths_nm = wavelengths;
	rgb_wavelengths_m = wavelengths * 1e-9;
}

void atmosphere_system::update_coefficients(ecs::entity entity)
{
	// Abort if entity has no atmosphere component
	if (!registry.has<atmosphere_component>(entity))
		return;
	
	// Get atmosphere component of the entity
	atmosphere_component& atmosphere = registry.get<atmosphere_component>(entity);
	
	// Calculate polarization factors
	const double rayleigh_polarization = physics::atmosphere::polarization(atmosphere.index_of_refraction, atmosphere.rayleigh_density);
	const double mie_polarization = physics::atmosphere::polarization(atmosphere.index_of_refraction, atmosphere.mie_density);
	
	// Calculate Rayleigh scattering coefficients
	atmosphere.rayleigh_scattering =
	{
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.x, atmosphere.rayleigh_density, rayleigh_polarization),
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.y, atmosphere.rayleigh_density, rayleigh_polarization),
		physics::atmosphere::scattering_rayleigh(rgb_wavelengths_m.z, atmosphere.rayleigh_density, rayleigh_polarization)
	};
	
	// Calculate Mie scattering coefficients
	const double mie_scattering = physics::atmosphere::scattering_mie(atmosphere.mie_density, mie_polarization);
	atmosphere.mie_scattering = 
	{
		mie_scattering,
		mie_scattering,
		mie_scattering
	};
}

void atmosphere_system::on_atmosphere_construct(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere)
{
	update_coefficients(entity);
}

void atmosphere_system::on_atmosphere_replace(ecs::registry& registry, ecs::entity entity, ecs::atmosphere_component& atmosphere)
{
	update_coefficients(entity);
}

} // namespace ecs

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

#include "entity/systems/atmosphere.hpp"
#include "physics/gas/atmosphere.hpp"
#include "physics/gas/ozone.hpp"
#include "physics/number-density.hpp"

namespace entity {
namespace system {

atmosphere::atmosphere(entity::registry& registry):
	updatable(registry),
	rgb_wavelengths{0, 0, 0},
	rgb_ozone_cross_sections{0, 0, 0},
	atmosphere_component(nullptr),
	sky_pass(nullptr)
{
	registry.on_construct<entity::component::atmosphere>().connect<&atmosphere::on_atmosphere_construct>(this);
	registry.on_replace<entity::component::atmosphere>().connect<&atmosphere::on_atmosphere_replace>(this);
	registry.on_destroy<entity::component::atmosphere>().connect<&atmosphere::on_atmosphere_destroy>(this);
}

void atmosphere::update(double t, double dt)
{}

void atmosphere::set_rgb_wavelengths(const double3& wavelengths)
{
	rgb_wavelengths = wavelengths;
	
	// Update ozone cross sections
	rgb_ozone_cross_sections =
	{
		physics::gas::ozone::cross_section_293k<double>(wavelengths.x * 1e9),
		physics::gas::ozone::cross_section_293k<double>(wavelengths.y * 1e9),
		physics::gas::ozone::cross_section_293k<double>(wavelengths.z * 1e9)
	};
	
	atmosphere_modified();
}

void atmosphere::set_sky_pass(::render::sky_pass* pass)
{
	sky_pass = pass;
	update_sky_pass();
}

void atmosphere::atmosphere_modified()
{
	if (!atmosphere_component)
		return;
	
	// Get atmosphere component of the entity
	entity::component::atmosphere& component = *atmosphere_component;
	
	// Calculate Rayleigh scattering coefficients
	const double rayleigh_density = physics::number_density(component.rayleigh_concentration);
	const double rayleigh_polarization = physics::gas::atmosphere::polarization(component.index_of_refraction, rayleigh_density);
	component.rayleigh_scattering =
	{
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, rgb_wavelengths.x),
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, rgb_wavelengths.y),
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, rgb_wavelengths.z)
	};
	
	// Calculate Mie scattering and extinction coefficients
	const double mie_density = physics::number_density(component.mie_concentration);
	const double mie_polarization = physics::gas::atmosphere::polarization(component.index_of_refraction, mie_density);
	component.mie_scattering = physics::gas::atmosphere::scattering(mie_density, mie_polarization);
	component.mie_extinction = physics::gas::atmosphere::extinction(component.mie_scattering, component.mie_albedo);
	
	// Calculate ozone absorption coefficients
	const double ozone_density = physics::number_density(component.ozone_concentration);
	component.ozone_absorption =
	{
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.x, ozone_density),
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.y, ozone_density),
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.z, ozone_density)
	};
	
	// Pass atmosphere parameters to sky pass
	update_sky_pass();
}

void atmosphere::update_sky_pass()
{
	if (!sky_pass || !atmosphere_component)
		return;
	
	const entity::component::atmosphere& component = *atmosphere_component;
	
	sky_pass->set_atmosphere_upper_limit(static_cast<float>(component.upper_limit));
	sky_pass->set_rayleigh_parameters(static_cast<float>(component.rayleigh_scale_height), math::type_cast<float>(component.rayleigh_scattering));
	sky_pass->set_mie_parameters(static_cast<float>(component.mie_scale_height), static_cast<float>(component.mie_scattering), static_cast<float>(component.mie_extinction), static_cast<float>(component.mie_anisotropy));
	sky_pass->set_ozone_parameters(static_cast<float>(component.ozone_lower_limit), static_cast<float>(component.ozone_upper_limit), static_cast<float>(component.ozone_mode), math::type_cast<float>(component.ozone_absorption));
	sky_pass->set_airglow_illuminance(math::type_cast<float>(component.airglow_illuminance));
}

void atmosphere::on_atmosphere_construct(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& component)
{
	atmosphere_component = &component;
	atmosphere_modified();
}

void atmosphere::on_atmosphere_replace(entity::registry& registry, entity::id entity_id, entity::component::atmosphere& component)
{
	atmosphere_component = &component;
	atmosphere_modified();
}

void atmosphere::on_atmosphere_destroy(entity::registry& registry, entity::id entity_id)
{
	atmosphere_component = nullptr;
}

} // namespace system
} // namespace entity

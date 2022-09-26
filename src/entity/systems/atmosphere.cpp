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
#include "color/srgb.hpp"

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
	atmosphere_modified();
}

void atmosphere::set_rgb_ozone_cross_sections(const double3& cross_sections)
{
	rgb_ozone_cross_sections = cross_sections;
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
	
	// Calculate Rayleigh scattering coefficients for sRGB wavelengths
	const double rayleigh_polarization = physics::gas::atmosphere::polarization(component.index_of_refraction, component.rayleigh_density);
	const double3 rayleigh_scattering_srgb =
	{
		physics::gas::atmosphere::scattering_rayleigh(rgb_wavelengths.x, component.rayleigh_density, rayleigh_polarization),
		physics::gas::atmosphere::scattering_rayleigh(rgb_wavelengths.y, component.rayleigh_density, rayleigh_polarization),
		physics::gas::atmosphere::scattering_rayleigh(rgb_wavelengths.z, component.rayleigh_density, rayleigh_polarization)
	};
	
	// Transform Rayleigh scattering coefficients from sRGB to ACEScg
	component.rayleigh_scattering = color::srgb::to_acescg(rayleigh_scattering_srgb);
	
	// Calculate Mie scattering coefficient
	const double mie_polarization = physics::gas::atmosphere::polarization(component.index_of_refraction, component.mie_density);
	component.mie_scattering = physics::gas::atmosphere::scattering_mie(component.mie_density, mie_polarization);
	
	// Calculate Mie absorption coefficient
	component.mie_absorption = physics::gas::atmosphere::absorption_mie(component.mie_scattering);
	
	// Calculate ozone absorption coefficients for sRGB wavelengths
	const double air_number_density = physics::number_density(component.air_concentration);
	const double3 ozone_absorption_srgb =
	{
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.x, air_number_density, component.ozone_concentration),
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.y, air_number_density, component.ozone_concentration),
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.z, air_number_density, component.ozone_concentration)
	};
	
	// Transform ozone absorption coefficients from sRGB to ACEScg
	component.ozone_absorption = color::srgb::to_acescg(ozone_absorption_srgb);
	
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
	sky_pass->set_mie_parameters(static_cast<float>(component.mie_scale_height), static_cast<float>(component.mie_scattering), static_cast<float>(component.mie_absorption), static_cast<float>(component.mie_anisotropy));
	sky_pass->set_ozone_parameters(static_cast<float>(component.ozone_lower_limit), static_cast<float>(component.ozone_upper_limit), static_cast<float>(component.ozone_mode), math::type_cast<float>(component.ozone_absorption));
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

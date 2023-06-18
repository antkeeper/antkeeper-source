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

#include "game/systems/atmosphere-system.hpp"
#include <engine/physics/gas/atmosphere.hpp>
#include <engine/physics/gas/ozone.hpp>
#include <engine/physics/number-density.hpp>


atmosphere_system::atmosphere_system(entity::registry& registry):
	updatable_system(registry),
	rgb_wavelengths{0, 0, 0},
	rgb_ozone_cross_sections{0, 0, 0},
	active_atmosphere_eid(entt::null),
	sky_pass(nullptr)
{
	registry.on_construct<::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_construct>(this);
	registry.on_update<::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_update>(this);
	registry.on_destroy<::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_destroy>(this);
}

atmosphere_system::~atmosphere_system()
{
	registry.on_construct<::atmosphere_component>().disconnect<&atmosphere_system::on_atmosphere_construct>(this);
	registry.on_update<::atmosphere_component>().disconnect<&atmosphere_system::on_atmosphere_update>(this);
	registry.on_destroy<::atmosphere_component>().disconnect<&atmosphere_system::on_atmosphere_destroy>(this);
}

void atmosphere_system::update(float t, float dt)
{}

void atmosphere_system::set_rgb_wavelengths(const double3& wavelengths)
{
	rgb_wavelengths = wavelengths;
	
	// Update ozone cross sections
	rgb_ozone_cross_sections =
	{
		physics::gas::ozone::cross_section_293k<double>(wavelengths.x() * 1e9),
		physics::gas::ozone::cross_section_293k<double>(wavelengths.y() * 1e9),
		physics::gas::ozone::cross_section_293k<double>(wavelengths.z() * 1e9)
	};
	
	// Update atmosphere components
	registry.view<::atmosphere_component>().each
	(
		[&](entity::id entity_id, auto& component)
		{
			update_atmosphere(entity_id);
		}
	);
}

void atmosphere_system::set_sky_pass(::render::sky_pass* pass)
{
	sky_pass = pass;
	update_sky_pass();
}

void atmosphere_system::set_active_atmosphere(entity::id entity_id)
{
	if (entity_id != active_atmosphere_eid)
	{
		active_atmosphere_eid = entity_id;
		update_sky_pass();
	}
}

void atmosphere_system::update_atmosphere(entity::id entity_id)
{
	// Get atmosphere component of the entity
	::atmosphere_component* component = registry.try_get<::atmosphere_component>(entity_id);
	
	// Abort if entity has no atmosphere component
	if (!component)
		return;
	
	// Calculate Rayleigh scattering coefficients
	const double rayleigh_density = physics::number_density(component->rayleigh_concentration);
	const double rayleigh_polarization = physics::gas::atmosphere::polarization(component->index_of_refraction, rayleigh_density);
	component->rayleigh_scattering =
	{
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, rgb_wavelengths.x()),
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, rgb_wavelengths.y()),
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, rgb_wavelengths.z())
	};
	
	// Calculate Mie scattering and extinction coefficients
	const double mie_density = physics::number_density(component->mie_concentration);
	const double mie_polarization = physics::gas::atmosphere::polarization(component->index_of_refraction, mie_density);
	component->mie_scattering = physics::gas::atmosphere::scattering(mie_density, mie_polarization);
	component->mie_extinction = physics::gas::atmosphere::extinction(component->mie_scattering, component->mie_albedo);
	
	// Calculate ozone absorption coefficients
	const double ozone_density = physics::number_density(component->ozone_concentration);
	component->ozone_absorption =
	{
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.x(), ozone_density),
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.y(), ozone_density),
		physics::gas::ozone::absorption(rgb_ozone_cross_sections.z(), ozone_density)
	};
	
	// Update sky pass parameters
	if (entity_id == active_atmosphere_eid)
	{
		update_sky_pass();
	}
}

void atmosphere_system::update_sky_pass()
{
	// Abort if no sky pass set
	if (!sky_pass)
		return;
	
	// Abort if active atmosphere entity is not valid
	if (!registry.valid(active_atmosphere_eid))
		return;
	
	// Get atmosphere component of the entity
	::atmosphere_component* component = registry.try_get<::atmosphere_component>(active_atmosphere_eid);
	
	// Abort if entity has no atmosphere component
	if (!component)
		return;
	
	sky_pass->set_atmosphere_upper_limit(static_cast<float>(component->upper_limit));
	sky_pass->set_rayleigh_parameters(static_cast<float>(component->rayleigh_scale_height), math::vector<float, 3>(component->rayleigh_scattering));
	sky_pass->set_mie_parameters(static_cast<float>(component->mie_scale_height), static_cast<float>(component->mie_scattering), static_cast<float>(component->mie_extinction), static_cast<float>(component->mie_anisotropy));
	sky_pass->set_ozone_parameters(static_cast<float>(component->ozone_lower_limit), static_cast<float>(component->ozone_upper_limit), static_cast<float>(component->ozone_mode), math::vector<float, 3>(component->ozone_absorption));
	sky_pass->set_airglow_luminance(math::vector<float, 3>(component->airglow_luminance));
}

void atmosphere_system::on_atmosphere_construct(entity::registry& registry, entity::id entity_id)
{
	update_atmosphere(entity_id);
}

void atmosphere_system::on_atmosphere_update(entity::registry& registry, entity::id entity_id)
{
	update_atmosphere(entity_id);
}

void atmosphere_system::on_atmosphere_destroy(entity::registry& registry, entity::id entity_id)
{
	if (entity_id == active_atmosphere_eid)
		active_atmosphere_eid = entt::null;
}


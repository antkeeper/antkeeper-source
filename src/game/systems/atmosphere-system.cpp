// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/atmosphere-system.hpp"
import engine.physics.gas.atmosphere;
import engine.physics.gas.ozone;
import engine.physics.number_density;

atmosphere_system::atmosphere_system(entity::registry& registry):
	m_registry(registry)
{
	m_registry.on_construct<::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_construct>(this);
	m_registry.on_update<::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_update>(this);
	m_registry.on_destroy<::atmosphere_component>().connect<&atmosphere_system::on_atmosphere_destroy>(this);
	
	set_rgb_wavelengths({680, 550, 440});

	// Wavelengths of the ITU-R BT.2020 primaries
	// set_rgb_wavelengths({630, 532, 467});
}

atmosphere_system::~atmosphere_system()
{
	m_registry.on_construct<::atmosphere_component>().disconnect<&atmosphere_system::on_atmosphere_construct>(this);
	m_registry.on_update<::atmosphere_component>().disconnect<&atmosphere_system::on_atmosphere_update>(this);
	m_registry.on_destroy<::atmosphere_component>().disconnect<&atmosphere_system::on_atmosphere_destroy>(this);
}

void atmosphere_system::fixed_update(entity::registry&, float, float)
{}

void atmosphere_system::set_rgb_wavelengths(const math::dvec3& wavelengths)
{
	m_rgb_wavelengths_nm = wavelengths;
	m_rgb_wavelengths_m = m_rgb_wavelengths_nm * 1e-9;
	
	// Update ozone cross sections
	m_rgb_ozone_cross_sections =
	{
		physics::gas::ozone::cross_section_293k<double>(m_rgb_wavelengths_nm.x()),
		physics::gas::ozone::cross_section_293k<double>(m_rgb_wavelengths_nm.y()),
		physics::gas::ozone::cross_section_293k<double>(m_rgb_wavelengths_nm.z())
	};
	
	// Update atmosphere components
	m_registry.view<::atmosphere_component>().each
	(
		[&](entity::id entity_id, auto&)
		{
			update_atmosphere(entity_id);
		}
	);
}

void atmosphere_system::set_sky_pass(::render::sky_pass* pass)
{
	m_sky_pass = pass;
	update_sky_pass();
}

void atmosphere_system::set_active_atmosphere(entity::id entity_id)
{
	if (entity_id != m_active_atmosphere_eid)
	{
		m_active_atmosphere_eid = entity_id;
		update_sky_pass();
	}
}

void atmosphere_system::update_atmosphere(entity::id entity_id)
{
	// Get atmosphere component of the entity
	::atmosphere_component* component = m_registry.try_get<::atmosphere_component>(entity_id);
	
	// Abort if entity has no atmosphere component
	if (!component)
	{
		return;
	}
	
	// Calculate Rayleigh scattering coefficients
	const double rayleigh_density = physics::number_density(component->rayleigh_concentration);
	const double rayleigh_polarization = physics::gas::atmosphere::polarization(component->index_of_refraction, rayleigh_density);
	component->rayleigh_scattering =
	{
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, m_rgb_wavelengths_m.x()),
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, m_rgb_wavelengths_m.y()),
		physics::gas::atmosphere::scattering(rayleigh_density, rayleigh_polarization, m_rgb_wavelengths_m.z())
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
		physics::gas::ozone::absorption(m_rgb_ozone_cross_sections.x(), ozone_density),
		physics::gas::ozone::absorption(m_rgb_ozone_cross_sections.y(), ozone_density),
		physics::gas::ozone::absorption(m_rgb_ozone_cross_sections.z(), ozone_density)
	};
	
	// Update sky pass parameters
	if (entity_id == m_active_atmosphere_eid)
	{
		update_sky_pass();
	}
}

void atmosphere_system::update_sky_pass()
{
	// Abort if no sky pass set
	if (!m_sky_pass)
	{
		return;
	}
	
	// Abort if active atmosphere entity is not valid
	if (!m_registry.valid(m_active_atmosphere_eid))
	{
		return;
	}
	
	// Get atmosphere component of the entity
	::atmosphere_component* component = m_registry.try_get<::atmosphere_component>(m_active_atmosphere_eid);
	
	// Abort if entity has no atmosphere component
	if (!component)
	{
		return;
	}
	
	m_sky_pass->set_atmosphere_upper_limit(static_cast<float>(component->upper_limit));
	m_sky_pass->set_rayleigh_parameters(static_cast<float>(component->rayleigh_scale_height), math::fvec3(component->rayleigh_scattering));
	m_sky_pass->set_mie_parameters(static_cast<float>(component->mie_scale_height), static_cast<float>(component->mie_scattering), static_cast<float>(component->mie_extinction), static_cast<float>(component->mie_anisotropy));
	m_sky_pass->set_ozone_parameters(static_cast<float>(component->ozone_lower_limit), static_cast<float>(component->ozone_upper_limit), static_cast<float>(component->ozone_mode), math::fvec3(component->ozone_absorption));
	m_sky_pass->set_airglow_luminance(math::fvec3(component->airglow_luminance));
}

void atmosphere_system::on_atmosphere_construct(entity::registry&, entity::id entity_id)
{
	update_atmosphere(entity_id);
}

void atmosphere_system::on_atmosphere_update(entity::registry&, entity::id entity_id)
{
	update_atmosphere(entity_id);
}

void atmosphere_system::on_atmosphere_destroy(entity::registry&, entity::id entity_id)
{
	if (entity_id == m_active_atmosphere_eid)
	{
		m_active_atmosphere_eid = entt::null;
	}
}

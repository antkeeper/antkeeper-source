// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include "game/systems/blackbody-system.hpp"
#include <engine/physics/light/blackbody.hpp>
#include <engine/physics/light/photometry.hpp>
#include <engine/math/quadrature.hpp>
#include <engine/color/xyz.hpp>
#include <engine/color/bt2020.hpp>
#include <engine/color/illuminants.hpp>
#include <numeric>

blackbody_system::blackbody_system(entity::registry& registry):
	updatable_system(registry)
{
	// Construct a range of sample wavelengths in the visible spectrum
	m_visible_wavelengths_nm.resize(780 - 280);
	std::iota(m_visible_wavelengths_nm.begin(), m_visible_wavelengths_nm.end(), 280);
	
	m_registry.on_construct<::blackbody_component>().connect<&blackbody_system::on_blackbody_construct>(this);
	m_registry.on_update<::blackbody_component>().connect<&blackbody_system::on_blackbody_update>(this);
}

blackbody_system::~blackbody_system()
{
	m_registry.on_construct<::blackbody_component>().disconnect<&blackbody_system::on_blackbody_construct>(this);
	m_registry.on_update<::blackbody_component>().disconnect<&blackbody_system::on_blackbody_update>(this);
}

void blackbody_system::update([[maybe_unused]] float t, [[maybe_unused]] float dt)
{}

void blackbody_system::update_blackbody(entity::id entity_id)
{
	// Get blackbody component
	auto& blackbody = m_registry.get<blackbody_component>(entity_id);
	
	// Construct a lambda function which calculates the blackbody's RGB luminance of a given wavelength
	auto rgb_spectral_luminance = [&](double wavelength_nm) -> math::dvec3
	{
		// Convert wavelength from nanometers to meters
		const double wavelength_m = wavelength_nm * 1e-9;
		
		// Calculate the spectral radiance of the wavelength
		const double spectral_radiance = physics::light::blackbody::spectral_radiance<double>(blackbody.temperature, wavelength_m);
		
		// Convert spectral radiance to spectral luminance
		const double spectral_luminance = spectral_radiance * 1e-9 * physics::light::max_luminous_efficacy<double>;
		
		// Calculate the XYZ color of the wavelength using CIE color matching functions then transform to RGB
		const auto color_xyz = color::xyz_match(wavelength_nm);
		const auto color_rgb = color::bt2020<double>.xyz_to_rgb(color_xyz);

		// constexpr auto illuminant_e_to_d65 = color::cat_matrix<double>(color::deg2_e<double>, color::bt2020<double>.white_point);
		// const auto color_rgb = color::bt2020<double>.xyz_to_rgb(illuminant_e_to_d65 * color_xyz);
		
		// Scale RGB color by spectral luminance
		return color_rgb * spectral_luminance;
	};
	
	// Integrate the blackbody RGB spectral luminance over wavelengths in the visible spectrum
	const math::dvec3 rgb_luminance = math::simpson(rgb_spectral_luminance, m_visible_wavelengths_nm.begin(), m_visible_wavelengths_nm.end());
	
	// Extract luminance and color from RGB luminance
	blackbody.luminance = math::max_element(rgb_luminance);
	blackbody.color = rgb_luminance / blackbody.luminance;
}

void blackbody_system::on_blackbody_construct([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	update_blackbody(entity_id);
}

void blackbody_system::on_blackbody_update([[maybe_unused]] entity::registry& registry, entity::id entity_id)
{
	update_blackbody(entity_id);
}

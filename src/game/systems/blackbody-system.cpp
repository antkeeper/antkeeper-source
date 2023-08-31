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

#include "game/systems/blackbody-system.hpp"
#include <engine/physics/light/blackbody.hpp>
#include <engine/physics/light/photometry.hpp>
#include <engine/math/quadrature.hpp>
#include <engine/config.hpp>
#include <engine/color/xyz.hpp>
#include <numeric>

blackbody_system::blackbody_system(entity::registry& registry):
	updatable_system(registry)
{
	// Construct a range of sample wavelengths in the visible spectrum
	m_visible_wavelengths_nm.resize(780 - 280);
	std::iota(m_visible_wavelengths_nm.begin(), m_visible_wavelengths_nm.end(), 280);
	
	registry.on_construct<::blackbody_component>().connect<&blackbody_system::on_blackbody_construct>(this);
	registry.on_update<::blackbody_component>().connect<&blackbody_system::on_blackbody_update>(this);
}

blackbody_system::~blackbody_system()
{
	registry.on_construct<::blackbody_component>().disconnect<&blackbody_system::on_blackbody_construct>(this);
	registry.on_update<::blackbody_component>().disconnect<&blackbody_system::on_blackbody_update>(this);
}

void blackbody_system::update(float t, float dt)
{}

void blackbody_system::update_blackbody(entity::id entity_id)
{
	// Get blackbody component
	auto& blackbody = registry.get<blackbody_component>(entity_id);
	
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
		const auto color_rgb = config::scene_linear_color_space<double>.from_xyz * color_xyz;
		
		// Scale RGB color by spectral luminance
		return color_rgb * spectral_luminance;
	};
	
	// Integrate the blackbody RGB spectral luminance over wavelengths in the visible spectrum
	const math::dvec3 rgb_luminance = math::quadrature::simpson(rgb_spectral_luminance, m_visible_wavelengths_nm.begin(), m_visible_wavelengths_nm.end());
	
	// Extract luminance and color from RGB luminance
	blackbody.luminance = math::max(rgb_luminance);
	blackbody.color = rgb_luminance / blackbody.luminance;
}

void blackbody_system::on_blackbody_construct(entity::registry& registry, entity::id entity_id)
{
	update_blackbody(entity_id);
}

void blackbody_system::on_blackbody_update(entity::registry& registry, entity::id entity_id)
{
	update_blackbody(entity_id);
}

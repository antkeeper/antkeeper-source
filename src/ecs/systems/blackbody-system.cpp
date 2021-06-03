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

#include "ecs/systems/blackbody-system.hpp"
#include "color/color.hpp"
#include "physics/light/blackbody.hpp"
#include "physics/light/photometry.hpp"
#include "math/quadrature.hpp"

namespace ecs {

blackbody_system::blackbody_system(ecs::registry& registry):
	entity_system(registry)
{
	// RGB wavelengths determined by matching wavelengths to XYZ, transforming XYZ to ACEScg, then selecting the max wavelengths for R, G, and B.
	rgb_wavelengths_nm = {602.224, 541.069, 448.143};
	rgb_wavelengths_m = rgb_wavelengths_nm * 1e-9;
	
	// Construct a range of sample wavelengths in the visible spectrum
	visible_wavelengths_nm.resize(780 - 280);
	std::iota(visible_wavelengths_nm.begin(), visible_wavelengths_nm.end(), 280);
	
	registry.on_construct<ecs::blackbody_component>().connect<&blackbody_system::on_blackbody_construct>(this);
	registry.on_replace<ecs::blackbody_component>().connect<&blackbody_system::on_blackbody_replace>(this);
	
	registry.on_construct<ecs::celestial_body_component>().connect<&blackbody_system::on_celestial_body_construct>(this);
	registry.on_replace<ecs::celestial_body_component>().connect<&blackbody_system::on_celestial_body_replace>(this);
}

void blackbody_system::update(double t, double dt)
{}

void blackbody_system::update_luminous_intensity(ecs::entity entity)
{
	// Abort if entity has no blackbody component
	if (!registry.has<blackbody_component>(entity))
		return;
	
	// Get blackbody component of the entity
	blackbody_component& blackbody = registry.get<blackbody_component>(entity);
	
	// Clear luminous intensity
	blackbody.luminous_intensity = {0, 0, 0};
	
	// Abort if entity has no celestial body component
	if (!registry.has<celestial_body_component>(entity))
		return;
	
	// Get celestial body component of the entity
	const celestial_body_component& celestial_body = registry.get<celestial_body_component>(entity);
	
	// Calculate (spherical) surface area of the celestial body
	const double surface_area = 4.0 * math::pi<double> * celestial_body.radius * celestial_body.radius;
	
	// Construct a lambda function which calculates the blackbody's RGB luminous intensity of a given wavelength
	auto rgb_luminous_intensity = [blackbody, surface_area](double wavelength_nm) -> double3
	{
		// Convert wavelength from nanometers to meters
		const double wavelength_m = wavelength_nm * 1e-9;
		
		// Calculate the spectral intensity of the wavelength
		const double spectral_intensity = physics::light::blackbody::spectral_intensity<double>(blackbody.temperature, surface_area, wavelength_m);
		
		// Calculate the ACEScg color of the wavelength using CIE color matching functions
		double3 spectral_color = color::xyz::to_acescg(color::xyz::match(wavelength_nm));
		
		// Scale the spectral color by spectral intensity
		return spectral_color * spectral_intensity * 1e-9 * physics::light::max_luminous_efficacy<double>;
	};
	
	// Integrate the blackbody RGB luminous intensity over wavelengths in the visible spectrum
	blackbody.luminous_intensity = math::quadrature::simpson(rgb_luminous_intensity, visible_wavelengths_nm.begin(), visible_wavelengths_nm.end());
}

void blackbody_system::on_blackbody_construct(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody)
{
	update_luminous_intensity(entity);
}

void blackbody_system::on_blackbody_replace(ecs::registry& registry, ecs::entity entity, ecs::blackbody_component& blackbody)
{
	update_luminous_intensity(entity);
}

void blackbody_system::on_celestial_body_construct(ecs::registry& registry, ecs::entity entity, ecs::celestial_body_component& celestial_body)
{
	update_luminous_intensity(entity);
}

void blackbody_system::on_celestial_body_replace(ecs::registry& registry, ecs::entity entity, ecs::celestial_body_component& celestial_body)
{
	update_luminous_intensity(entity);
}

} // namespace ecs

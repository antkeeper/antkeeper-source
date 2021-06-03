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

#include "entity/systems/blackbody.hpp"
#include "color/color.hpp"
#include "physics/light/blackbody.hpp"
#include "physics/light/photometry.hpp"
#include "math/quadrature.hpp"

namespace entity {
namespace system {

blackbody::blackbody(entity::registry& registry):
	updatable(registry),
	rgb_wavelengths_nm{0, 0, 0},
	rgb_wavelengths_m{0, 0, 0}
{
	// Construct a range of sample wavelengths in the visible spectrum
	visible_wavelengths_nm.resize(780 - 280);
	std::iota(visible_wavelengths_nm.begin(), visible_wavelengths_nm.end(), 280);
	
	registry.on_construct<entity::component::blackbody>().connect<&blackbody::on_blackbody_construct>(this);
	registry.on_replace<entity::component::blackbody>().connect<&blackbody::on_blackbody_replace>(this);
	
	registry.on_construct<entity::component::celestial_body>().connect<&blackbody::on_celestial_body_construct>(this);
	registry.on_replace<entity::component::celestial_body>().connect<&blackbody::on_celestial_body_replace>(this);
}

void blackbody::update(double t, double dt)
{}

void blackbody::set_rgb_wavelengths(const double3& wavelengths)
{
	rgb_wavelengths_nm = wavelengths;
	rgb_wavelengths_m = wavelengths * 1e-9;
}

void blackbody::update_luminous_intensity(entity::id entity_id)
{
	// Abort if entity has no blackbody component
	if (!registry.has<component::blackbody>(entity_id))
		return;
	
	// Get blackbody component of the entity
	component::blackbody& blackbody = registry.get<component::blackbody>(entity_id);
	
	// Clear luminous intensity
	blackbody.luminous_intensity = {0, 0, 0};
	
	// Abort if entity has no celestial body component
	if (!registry.has<component::celestial_body>(entity_id))
		return;
	
	// Get celestial body component of the entity
	const component::celestial_body& celestial_body = registry.get<component::celestial_body>(entity_id);
	
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

void blackbody::on_blackbody_construct(entity::registry& registry, entity::id entity_id, entity::component::blackbody& blackbody)
{
	update_luminous_intensity(entity_id);
}

void blackbody::on_blackbody_replace(entity::registry& registry, entity::id entity_id, entity::component::blackbody& blackbody)
{
	update_luminous_intensity(entity_id);
}

void blackbody::on_celestial_body_construct(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body)
{
	update_luminous_intensity(entity_id);
}

void blackbody::on_celestial_body_replace(entity::registry& registry, entity::id entity_id, entity::component::celestial_body& celestial_body)
{
	update_luminous_intensity(entity_id);
}

} // namespace system
} // namespace entity

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
#include <engine/color/color.hpp>
#include <engine/physics/light/blackbody.hpp>
#include <engine/physics/light/photometry.hpp>
#include <engine/math/quadrature.hpp>
#include <numeric>


blackbody_system::blackbody_system(entity::registry& registry):
	updatable_system(registry),
	illuminant(color::illuminant::deg2::d50<double>)
{
	// Construct a range of sample wavelengths in the visible spectrum
	visible_wavelengths_nm.resize(780 - 280);
	std::iota(visible_wavelengths_nm.begin(), visible_wavelengths_nm.end(), 280);
	
	registry.on_construct<::blackbody_component>().connect<&blackbody_system::on_blackbody_construct>(this);
	registry.on_update<::blackbody_component>().connect<&blackbody_system::on_blackbody_update>(this);
	registry.on_construct<::celestial_body_component>().connect<&blackbody_system::on_celestial_body_construct>(this);
	registry.on_update<::celestial_body_component>().connect<&blackbody_system::on_celestial_body_update>(this);
}

blackbody_system::~blackbody_system()
{
	registry.on_construct<::blackbody_component>().disconnect<&blackbody_system::on_blackbody_construct>(this);
	registry.on_update<::blackbody_component>().disconnect<&blackbody_system::on_blackbody_update>(this);
	registry.on_construct<::celestial_body_component>().disconnect<&blackbody_system::on_celestial_body_construct>(this);
	registry.on_update<::celestial_body_component>().disconnect<&blackbody_system::on_celestial_body_update>(this);
}

void blackbody_system::update(double t, double dt)
{}

void blackbody_system::set_illuminant(const math::vector2<double>& illuminant)
{
	this->illuminant = illuminant;
}

void blackbody_system::update_luminance(entity::id entity_id)
{
	// Get blackbody and celestial body components of the entity
	auto [blackbody, celestial_body] = registry.try_get<blackbody_component, celestial_body_component>(entity_id);
	
	// Abort if entity is missing a blackbody or celestial body component
	if (!blackbody || !celestial_body)
		return;
	
	// Construct chromatic adaptation transform
	const double3x3 cat = color::cat::matrix(illuminant, color::aces::white_point<double>);
	
	// Construct a lambda function which calculates the blackbody's RGB luminance of a given wavelength
	auto rgb_luminance = [temperature = blackbody->temperature, cat](double wavelength_nm) -> double3
	{
		// Convert wavelength from nanometers to meters
		const double wavelength_m = wavelength_nm * 1e-9;
		
		// Calculate the spectral intensity of the wavelength
		const double spectral_radiance = physics::light::blackbody::spectral_radiance<double>(temperature, wavelength_m);
		
		
		// Calculate the ACEScg color of the wavelength using CIE color matching functions
		double3 spectral_color = color::aces::ap1<double>.from_xyz * cat * color::xyz::match(wavelength_nm);
		
		// Scale the spectral color by spectral intensity
		return spectral_color * spectral_radiance * 1e-9 * physics::light::max_luminous_efficacy<double>;
	};
	
	// Integrate the blackbody RGB luminance over wavelengths in the visible spectrum
	blackbody->luminance = math::quadrature::simpson(rgb_luminance, visible_wavelengths_nm.begin(), visible_wavelengths_nm.end());
}

void blackbody_system::on_blackbody_construct(entity::registry& registry, entity::id entity_id)
{
	update_luminance(entity_id);
}

void blackbody_system::on_blackbody_update(entity::registry& registry, entity::id entity_id)
{
	update_luminance(entity_id);
}

void blackbody_system::on_celestial_body_construct(entity::registry& registry, entity::id entity_id)
{
	update_luminance(entity_id);
}

void blackbody_system::on_celestial_body_update(entity::registry& registry, entity::id entity_id)
{
	update_luminance(entity_id);
}

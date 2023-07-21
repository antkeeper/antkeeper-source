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

#include <engine/scene/rectangle-light.hpp>
#include <engine/math/numbers.hpp>

namespace scene {

rectangle_light::rectangle_light()
{
	transformed();
}

void rectangle_light::transformed()
{
	const auto& transform = get_transform();
	
	// Update corner positions
	// m_corners[0] = transform * math::fvec3{-0.5f, 0.0f, -0.5f};
	// m_corners[1] = transform * math::fvec3{ 0.5f, 0.0f, -0.5f};
	// m_corners[2] = transform * math::fvec3{ 0.5f, 0.0f,  0.5f};
	// m_corners[3] = transform * math::fvec3{-0.5f, 0.0f,  0.5f};
	m_corners[0] = transform * math::fvec3{-0.5f, -0.5f, 0.0f};
	m_corners[1] = transform * math::fvec3{-0.5f,  0.5f, 0.0f};
	m_corners[2] = transform * math::fvec3{ 0.5f,  0.5f, 0.0f};
	m_corners[3] = transform * math::fvec3{ 0.5f, -0.5f, 0.0f};
	
	// Update area
	m_area = get_scale().x() * get_scale().z();
	area_updated();
}

void rectangle_light::area_updated()
{
	// Calculate luminance from luminous flux
	m_luminance = m_luminous_flux / (m_area * math::pi<float>);
	m_colored_luminance = m_color * m_luminance;
}

void rectangle_light::color_updated()
{
	m_colored_luminous_flux = m_color * m_luminous_flux;
	m_colored_luminance = m_color * m_luminance;
}

void rectangle_light::luminous_flux_updated()
{
	m_colored_luminous_flux = m_color * m_luminous_flux;
	
	// Calculate luminance from luminous flux
	m_luminance = m_luminous_flux / (m_area * math::pi<float>);
	m_colored_luminance = m_color * m_luminance;
}

void rectangle_light::luminance_updated()
{
	m_colored_luminance = m_color * m_luminance;
	
	// Calculate luminous flux from luminance
	m_luminous_flux = m_luminance * (m_area * math::pi<float>);
	m_colored_luminous_flux = m_color * m_luminous_flux;
}

} // namespace scene

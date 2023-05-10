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

#include <engine/scene/sphere-light.hpp>
#include <engine/math/numbers.hpp>

namespace scene {

float sphere_light::get_luminance() const noexcept
{
	return m_luminous_power / (4.0f * m_radius * m_radius * math::sqr_pi<float>);
}

math::vector<float, 3> sphere_light::get_spectral_luminance() const noexcept
{
	return m_color * get_luminance();
}

} // namespace scene

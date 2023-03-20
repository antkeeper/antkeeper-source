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

#include <engine/scene/spot-light.hpp>
#include <cmath>

namespace scene {

void spot_light::set_cutoff(const math::vector<float, 2>& cutoff)
{
	m_cutoff = cutoff;
	m_cosine_cutoff = {std::cos(m_cutoff.x()), std::cos(m_cutoff.y())};
}

void spot_light::transformed()
{
	m_direction = get_transform().rotation * math::vector<float, 3>{0.0f, 0.0f, -1.0f};
}

} // namespace scene

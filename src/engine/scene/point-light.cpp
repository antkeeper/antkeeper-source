// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/point-light.hpp>

namespace scene {

void point_light::color_updated()
{
	m_colored_luminous_flux = get_color() * m_luminous_flux;
}

void point_light::luminous_flux_updated() noexcept
{
	m_colored_luminous_flux = get_color() * m_luminous_flux;
}

} // namespace scene

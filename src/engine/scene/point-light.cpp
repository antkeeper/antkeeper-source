// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.point_light;

namespace engine::scene
{
	void point_light::color_updated()
	{
		m_colored_luminous_flux = get_color() * m_luminous_flux;
	}

	void point_light::luminous_flux_updated() noexcept
	{
		m_colored_luminous_flux = get_color() * m_luminous_flux;
	}
}

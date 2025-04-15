// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.light;
import engine.color.bt2020;
import engine.color.cct;

namespace engine::scene
{
	void light::set_color(const math::fvec3& color)
	{
		m_color = color;
		color_updated();
	}

	void light::set_color_temperature(float temperature)
	{
		set_color(color::bt2020<float>.xyz_to_rgb(color::cct_to_xyz(temperature)));
	}

	void light::transformed()
	{
		m_bounds = {get_translation(), get_translation()};
	}
}

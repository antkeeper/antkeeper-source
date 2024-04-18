// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/light.hpp>
#include <engine/color/aces.hpp>
#include <engine/color/cct.hpp>

namespace scene {

void light::set_color(const math::fvec3& color)
{
	m_color = color;
	color_updated();
}

void light::set_color_temperature(float temperature)
{
	set_color(color::aces_ap1<float>.xyz_to_rgb(color::cct_to_xyz(temperature)));
}

void light::transformed()
{
	m_bounds = {get_translation(), get_translation()};
}

} // namespace scene

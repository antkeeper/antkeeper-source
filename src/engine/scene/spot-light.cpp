// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/spot-light.hpp>
#include <cmath>

namespace scene {

void spot_light::set_cutoff(const math::fvec2& cutoff)
{
	m_cutoff = cutoff;
	m_cosine_cutoff = {std::cos(m_cutoff.x()), std::cos(m_cutoff.y())};
}

void spot_light::transformed()
{
	m_direction = get_transform().rotation * math::fvec3{0.0f, 0.0f, -1.0f};
}

} // namespace scene

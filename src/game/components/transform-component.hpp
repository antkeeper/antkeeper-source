// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_TRANSFORM_COMPONENT_HPP
#define ANTKEEPER_GAME_TRANSFORM_COMPONENT_HPP

#include <engine/math/transform.hpp>

struct transform_component
{
	math::transform<float> local;
	math::transform<float> world;
};

#endif // ANTKEEPER_GAME_TRANSFORM_COMPONENT_HPP


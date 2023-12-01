// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_CAVITY_COMPONENT_HPP
#define ANTKEEPER_GAME_CAVITY_COMPONENT_HPP

#include <engine/math/vector.hpp>


struct cavity_component
{
	math::fvec3 position;
	float radius;
};


#endif // ANTKEEPER_GAME_CAVITY_COMPONENT_HPP


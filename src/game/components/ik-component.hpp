// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_IK_COMPONENT_HPP
#define ANTKEEPER_GAME_IK_COMPONENT_HPP

#include <engine/animation/ik/ik-rig.hpp>
#include <memory>

struct ik_component
{
	std::shared_ptr<ik_rig> rig;
};

#endif // ANTKEEPER_GAME_IK_COMPONENT_HPP

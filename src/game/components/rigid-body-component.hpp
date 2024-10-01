// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_RIGID_BODY_COMPONENT_HPP
#define ANTKEEPER_GAME_RIGID_BODY_COMPONENT_HPP

#include <engine/physics/kinematics/rigid-body.hpp>
#include <memory>

struct rigid_body_component
{
	std::unique_ptr<physics::rigid_body> body;
};

#endif // ANTKEEPER_GAME_RIGID_BODY_COMPONENT_HPP

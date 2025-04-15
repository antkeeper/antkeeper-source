// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_RIGID_BODY_COMPONENT_HPP
#define ANTKEEPER_GAME_RIGID_BODY_COMPONENT_HPP

import engine.physics.kinematics.rigid_body;
import <memory>;

using namespace engine;

struct rigid_body_component
{
	std::unique_ptr<physics::rigid_body> body;
};

#endif // ANTKEEPER_GAME_RIGID_BODY_COMPONENT_HPP

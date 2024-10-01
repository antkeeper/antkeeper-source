// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_RIGID_BODY_CONSTRAINT_COMPONENT_HPP
#define ANTKEEPER_GAME_RIGID_BODY_CONSTRAINT_COMPONENT_HPP

#include <engine/physics/kinematics/constraint.hpp>
#include <memory>

struct rigid_body_constraint_component
{
	std::unique_ptr<physics::constraint> constraint;
};

#endif // ANTKEEPER_GAME_RIGID_BODY_CONSTRAINT_COMPONENT_HPP

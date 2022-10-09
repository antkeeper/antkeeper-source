/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAME_COMPONENT_CONSTRAINT_HPP
#define ANTKEEPER_GAME_COMPONENT_CONSTRAINT_HPP

namespace game {
namespace component {

/// Transform constraint components.
namespace constraint {}

} // namespace component
} // namespace game

#include "game/component/constraint/child-of.hpp"
#include "game/component/constraint/copy-rotation.hpp"
#include "game/component/constraint/copy-scale.hpp"
#include "game/component/constraint/copy-transform.hpp"
#include "game/component/constraint/copy-translation.hpp"
#include "game/component/constraint/pivot.hpp"
#include "game/component/constraint/spring-rotation.hpp"
#include "game/component/constraint/spring-to.hpp"
#include "game/component/constraint/spring-translation.hpp"
#include "game/component/constraint/three-dof.hpp"
#include "game/component/constraint/track-to.hpp"

#endif // ANTKEEPER_GAME_COMPONENT_CONSTRAINT_HPP

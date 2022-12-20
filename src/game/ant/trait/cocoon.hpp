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

#ifndef ANTKEEPER_GAME_ANT_TRAIT_COCOON_HPP
#define ANTKEEPER_GAME_ANT_TRAIT_COCOON_HPP

#include "render/model.hpp"

namespace game {
namespace ant {
namespace trait {

/**
 * Ant cocoon description.
 */
struct cocoon
{
	/// 3D model of the cocoon, if present.
	render::model* model;
	
	/// Indicates whether a cocoon is formed by the larvae or not.
	bool present;
};

} // namespace trait
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_TRAIT_COCOON_HPP

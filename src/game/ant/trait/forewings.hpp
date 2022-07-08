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

#ifndef ANTKEEPER_GAME_ANT_TRAIT_FOREWINGS_HPP
#define ANTKEEPER_GAME_ANT_TRAIT_FOREWINGS_HPP

#include "render/model.hpp"

namespace game {
namespace ant {
namespace trait {

/**
 * Trait that describes the forewings of an ant.
 */
struct forewings
{	
	/// Wing length, measured in mesosomal lengths.
	float length;
	
	/// Wing width, measured in mesosomal lengths.
	float width;
	
	/// Degree of venation. A value of `1.0` indicates a highly-developed venation pattern, while `0.0` indicates a complete absence of visible venation.
	float venation;
	
	/// 3D model of the forewings.
	render::model* model;
};

} // namespace trait
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_TRAIT_FOREWINGS_HPP

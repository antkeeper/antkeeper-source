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

#ifndef ANTKEEPER_GAME_ANT_TRAIT_WAIST_HPP
#define ANTKEEPER_GAME_ANT_TRAIT_WAIST_HPP

#include "render/model.hpp"

namespace game {
namespace ant {
namespace trait {

/**
 * Trait that describes the waist (petiole plus postpetiole) of an ant.
 */
struct waist
{
	/// 3D model of the waist.
	render::model* model;
	
	/// Degree of spinescence.
	float spinescence;
	
	/// Postpetiole presence.
	bool postpetiole;
	
	/// Petiole length in dorsal view (PetL).
	float petiole_length;
	
	/// Petiole width in dorsal view (PetW).
	float petiole_width;
	
	/// Petiole height in in lateral profile (PetH).
	float petiole_height;
	
	/// Postpetiole length in dorsal view (PPL).
	float postpetiole_length;
	
	/// Postpetiole width in dorsal view (PPW).
	float postpetiole_width;
	
	/// Postpetiole height in in lateral profile (PPH).
	float postpetiole_height;
};

} // namespace trait
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_TRAIT_WAIST_HPP

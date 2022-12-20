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
 *
 * @see https://www.antwiki.org/wiki/Morphological_Measurements
 */
struct waist
{
	/// 3D model of the waist.
	render::model* model;
	
	//// Petiole presence.
	bool petiole_present;
	
	/// Petiole length, in mesosomal lengths.
	float petiole_length;
	
	/// Petiole width, in mesosomal lengths.
	float petiole_width;
	
	/// Petiole height, in mesosomal lengths.
	float petiole_height;
	
	/// Degree of petiole spinescence.
	float petiole_spinescence;
	
	/// Postpetiole presence.
	bool postpetiole_present;
	
	/// Postpetiole length, in mesosomal lengths.
	float postpetiole_length;
	
	/// Postpetiole width, in mesosomal lengths.
	float postpetiole_width;
	
	/// Postpetiole height, in mesosomal lengths.
	float postpetiole_height;
	
	/// Degree of postpetiole spinescence
	float postpetiole_spinescence;
};

} // namespace trait
} // namespace ant
} // namespace game

#endif // ANTKEEPER_GAME_ANT_TRAIT_WAIST_HPP

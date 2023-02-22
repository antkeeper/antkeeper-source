/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_GAME_COPY_TRANSLATION_CONSTRAINT_HPP
#define ANTKEEPER_GAME_COPY_TRANSLATION_CONSTRAINT_HPP

#include <engine/entity/id.hpp>


/**
 * Copies the translation of a target entity.
 */
struct copy_translation_constraint
{
	/// Target entity ID.
	entity::id target;
	
	/// Copy X translation.
	bool copy_x;
	
	/// Copy Y translation.
	bool copy_y;
	
	/// Copy Z translation.
	bool copy_z;
	
	/// Invert the copied X translation.
	bool invert_x;
	
	/// Invert the copied Y translation.
	bool invert_y;
	
	/// Invert the copied Z translation.
	bool invert_z;
	
	/// Add the copied translation.
	bool offset;
};


#endif // ANTKEEPER_GAME_COPY_TRANSLATION_CONSTRAINT_HPP

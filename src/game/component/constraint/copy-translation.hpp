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

#ifndef ANTKEEPER_GAME_COMPONENT_CONSTRAINT_COPY_TRANSLATION_HPP
#define ANTKEEPER_GAME_COMPONENT_CONSTRAINT_COPY_TRANSLATION_HPP

#include "entity/id.hpp"

namespace game {
namespace component {
namespace constraint {

/**
 * Copies the translation of a target entity.
 */
struct copy_translation
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

} // namespace constraint
} // namespace component
} // namespace game

#endif // ANTKEEPER_GAME_COMPONENT_CONSTRAINT_COPY_TRANSLATION_HPP

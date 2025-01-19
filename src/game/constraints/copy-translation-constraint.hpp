// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COPY_TRANSLATION_CONSTRAINT_HPP
#define ANTKEEPER_GAME_COPY_TRANSLATION_CONSTRAINT_HPP

#include <engine/entity/id.hpp>

/// Copies the translation of a target entity.
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

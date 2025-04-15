// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COPY_SCALE_CONSTRAINT_HPP
#define ANTKEEPER_GAME_COPY_SCALE_CONSTRAINT_HPP

import engine.entity.id;

using namespace engine;

/// Copies the scale of a target entity.
struct copy_scale_constraint
{
	/// Target entity ID.
	entity::id target;
	
	/// Copy X scale.
	bool copy_x;
	
	/// Copy Y scale.
	bool copy_y;
	
	/// Copy Z scale.
	bool copy_z;
};

#endif // ANTKEEPER_GAME_COPY_SCALE_CONSTRAINT_HPP

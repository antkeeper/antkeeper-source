// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_DECAY_COMPONENT_HPP
#define ANTKEEPER_GAME_DECAY_COMPONENT_HPP

/// Causes an entity to decay, deleting the entity when the decay is complete.
struct decay_component
{
	/// Decay rate.
	float rate{};
	
	/// Decay progress, on `[0, 1]`.
	float progress{};
};

#endif // ANTKEEPER_GAME_DECAY_COMPONENT_HPP

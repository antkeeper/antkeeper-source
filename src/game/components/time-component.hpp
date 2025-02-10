// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_TIME_COMPONENT_HPP
#define ANTKEEPER_GAME_TIME_COMPONENT_HPP

struct time_component
{
	/// Time scale, in simulated seconds per real-time second.
	float time_scale{1.0f};
};

#endif // ANTKEEPER_GAME_TIME_COMPONENT_HPP

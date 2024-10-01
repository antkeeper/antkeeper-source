// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_ISOMETRIC_GROWTH_COMPONENT_HPP
#define ANTKEEPER_GAME_ISOMETRIC_GROWTH_COMPONENT_HPP

/// Growth component which scales all body parts equally.
struct isometric_growth_component
{
	/// Growth rate.
	float rate{};
};

#endif // ANTKEEPER_GAME_ISOMETRIC_GROWTH_COMPONENT_HPP

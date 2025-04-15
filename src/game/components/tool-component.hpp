// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_TOOL_COMPONENT_HPP
#define ANTKEEPER_GAME_TOOL_COMPONENT_HPP

import <functional>;

struct tool_component
{
	std::function<void()> activated;
	std::function<void()> deactivated;
	std::function<void()> active;
};

#endif // ANTKEEPER_GAME_TOOL_COMPONENT_HPP


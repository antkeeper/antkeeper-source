// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_COMPONENT_SYSTEM_HPP
#define ANTKEEPER_GAME_COMPONENT_SYSTEM_HPP

/// Abstract base class for component systems.
class component_system
{
public:
	/// Destructs the component system.
	virtual ~component_system() = 0;
};

#endif // ANTKEEPER_GAME_COMPONENT_SYSTEM_HPP

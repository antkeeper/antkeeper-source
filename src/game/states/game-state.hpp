// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_STATE_HPP
#define ANTKEEPER_GAME_STATE_HPP

class game;

/// Abstract base class for game states.
class game_state
{
public:
	/// Constructs a game state.
	/// @param ctx Reference to the game context on which this state will operate.
	explicit game_state(::game& ctx);
	
	/// Destructs a game state.
	virtual ~game_state() = 0;
	
protected:
	::game& ctx;
};

#endif // ANTKEEPER_GAME_STATE_HPP

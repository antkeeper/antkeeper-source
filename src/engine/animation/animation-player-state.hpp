// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_PLAYBACK_STATE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_PLAYBACK_STATE_HPP

/// Animation player states.
enum class animation_player_state
{
	/// Animation player is stopped.
	stopped,
	
	/// Animation player is playing.
	playing,
	
	/// Animation player is paused.
	paused
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_PLAYBACK_STATE_HPP

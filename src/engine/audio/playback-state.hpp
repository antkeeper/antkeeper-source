// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


namespace engine::audio
{
	/// Playback states of a sound.
	enum class playback_state
	{
		/// Sound is stopped.
		stopped,
	
		/// Sound is playing.
		playing,
	
		/// Sound is paused.
		paused
	};
}

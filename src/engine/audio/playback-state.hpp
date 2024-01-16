// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AUDIO_PLAYBACK_STATE_HPP
#define ANTKEEPER_AUDIO_PLAYBACK_STATE_HPP

namespace audio {

/** Playback states of a sound. */
enum class playback_state
{
	/** Sound is stopped. */
	stopped,
	
	/** Sound is playing. */
	playing,
	
	/** Sound is paused. */
	paused
};

} // namespace audio

#endif // ANTKEEPER_AUDIO_PLAYBACK_STATE_HPP

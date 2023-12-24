// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AUDIO_PLAYBACK_STATE_HPP
#define ANTKEEPER_AUDIO_PLAYBACK_STATE_HPP

namespace audio {

/** Playback statse of a sound queue. */
enum class playback_state
{
	/** Sound que is stopped. */
	stopped,
	
	/** Sound que is playing. */
	playing,
	
	/** Sound que is paused. */
	paused
};

} // namespace audio

#endif // ANTKEEPER_AUDIO_PLAYBACK_STATE_HPP

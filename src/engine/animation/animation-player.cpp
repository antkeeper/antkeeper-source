// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation-player.hpp>
#include <algorithm>

void animation_player::advance(float seconds)
{
	// Prevent negative timesteps
	seconds = std::max(0.0f, seconds);

	if (!m_sequence)
	{
		// No active animation sequence, advance position and return
		m_position += seconds;
		return;
	}

	// Remember previous playback position
	const auto previous_position = m_position;

	// Advance playback position
	m_position += seconds;

	// Loop
	std::size_t loop_count = 0;
	if (m_position >= m_sequence_duration)
	{
		if (m_looping)
		{
			if (m_sequence_duration > 0.0f)
			{
				// Calculate looped position
				const auto looped_position = std::fmod(m_position, m_sequence_duration);

				// Calculate number of times looped
				loop_count = static_cast<std::size_t>(m_position / m_sequence_duration);

				// Set current position to looped position
				m_position = looped_position;
			}
			else
			{
				// Zero-duration looping sequence
				m_position = 0.0f;
			}
		}
		else if (m_autostop && m_position > m_sequence_duration)
		{
			m_state = animation_player_state::stopped;
		}
	}

	for (const auto& [path, track]: m_sequence->tracks())
	{
		if (!track.output())
		{
			// Ignore tracks with no output functions
			continue;
		}

		if (m_sample_buffer.size() < track.channels().size())
		{
			// Grow sample buffer to accommodate track channels
			m_sample_buffer.resize(track.channels().size());
		}

		// Sample track
		track.sample(m_position, m_sample_buffer);

		// Pass sample buffer and animation context to track output function
		track.output()(m_sample_buffer, m_context);
	}

	if (loop_count)
	{
		// Trigger cues on [previous position, m_sequence_duration)
		m_sequence->trigger_cues(previous_position, m_sequence_duration, m_context);

		// For each additional loop, trigger cues on [0, m_sequence_duration)
		for (std::size_t i = 1; i < loop_count; ++i)
		{
			m_sequence->trigger_cues(0.0f, m_sequence_duration, m_context);
		}

		// Trigger cues on [0, m_position)
		m_sequence->trigger_cues(0.0f, m_position, m_context);
	}
	else
	{
		// Trigger cues on [previous_position, m_position)
		m_sequence->trigger_cues(previous_position, m_position, m_context);
	}
}

void animation_player::play(std::shared_ptr<animation_sequence> sequence)
{
	m_sequence = std::move(sequence);
	
	// Determine duration of sequence
	if (m_sequence)
	{
		m_sequence_duration = m_sequence->duration();
	}
	else
	{
		m_sequence_duration = 0.0f;
	}

	m_state = animation_player_state::playing;
}

void animation_player::play()
{
	m_state = animation_player_state::playing;
}

void animation_player::stop()
{
	m_state = animation_player_state::stopped;
	m_position = 0.0f;
}

void animation_player::rewind()
{
	m_position = 0.0f;
}

void animation_player::pause()
{
	m_state = animation_player_state::paused;
}

void animation_player::seek(float seconds)
{
	m_position = seconds;
}

void animation_player::loop(bool enabled)
{
	m_looping = enabled;
}

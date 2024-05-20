// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/audio/sound-que.hpp>
#include <engine/math/functions.hpp>
#include <format>
#include <stdexcept>
#include <AL/al.h>

namespace audio {

sound_que::sound_que(std::shared_ptr<sound_wave> wave)
{
	// Generate source
	alGenSources(1, &m_al_source);
	if (auto error = alGetError(); error != AL_NO_ERROR)
	{
		throw std::runtime_error(std::format("OpenAL failed to generate source: {}", alGetString(error)));
	}
	
	// Set sound wave
	try
	{
		set_sound_wave(std::move(wave));
	}
	catch (...)
	{
		alDeleteSources(1, &m_al_source);
		throw;
	}
}

sound_que::~sound_que()
{
	alDeleteSources(1, &m_al_source);
}

void sound_que::play()
{
	alSourcePlay(m_al_source);
}

void sound_que::stop()
{
	alSourceStop(m_al_source);
}

void sound_que::rewind()
{
	alSourceRewind(m_al_source);
}

void sound_que::pause()
{
	alSourcePause(m_al_source);
}

void sound_que::seek_seconds(float seconds)
{
	alSourcef(m_al_source, AL_SEC_OFFSET, seconds);
	if (auto error = alGetError(); error != AL_NO_ERROR)
	{
		throw std::runtime_error(std::format("OpenAL failed to seek source to {} seconds: {}", seconds, alGetString(error)));
	}
}

void sound_que::seek_samples(std::size_t samples)
{
	alSourcei(m_al_source, AL_SAMPLE_OFFSET, static_cast<ALint>(samples));
	if (auto error = alGetError(); error != AL_NO_ERROR)
	{
		throw std::runtime_error(std::format("OpenAL failed to seek source to {} samples: {}", samples, alGetString(error)));
	}
}

void sound_que::seek_bytes(std::size_t bytes)
{
	alSourcei(m_al_source, AL_BYTE_OFFSET, static_cast<ALint>(bytes));
	if (auto error = alGetError(); error != AL_NO_ERROR)
	{
		throw std::runtime_error(std::format("OpenAL failed to seek source to {} bytes: {}", bytes, alGetString(error)));
	}
}

void sound_que::set_looping(bool looping)
{
	if (m_looping != looping)
	{
		m_looping = looping;
		alSourcei(m_al_source, AL_LOOPING, m_looping ? AL_TRUE : AL_FALSE);
	}
}

playback_state sound_que::get_playback_state() const
{
	ALint al_source_state;
	alGetSourcei(m_al_source, AL_SOURCE_STATE, &al_source_state);
	
	if (al_source_state == AL_PLAYING)
	{
		return playback_state::playing;
	}
	else if (al_source_state == AL_PAUSED)
	{
		return playback_state::paused;
	}
	else
	{
		return playback_state::stopped;
	}
}

float sound_que::get_playback_position_seconds() const
{
	ALfloat al_sec_offset;
	alGetSourcef(m_al_source, AL_SEC_OFFSET, &al_sec_offset);
	return al_sec_offset;
}
	
std::size_t sound_que::get_playback_position_samples() const
{
	ALint al_sample_offset;
	alGetSourcei(m_al_source, AL_SAMPLE_OFFSET, &al_sample_offset);
	return static_cast<std::size_t>(al_sample_offset);
}
	
std::size_t sound_que::get_playback_position_bytes() const
{
	ALint al_byte_offset;
	alGetSourcei(m_al_source, AL_BYTE_OFFSET, &al_byte_offset);
	return static_cast<std::size_t>(al_byte_offset);
}

void sound_que::set_position(const math::fvec3& position)
{
	if (m_position != position)
	{
		m_position = position;
		alSourcefv(m_al_source, AL_POSITION, m_position.data());
	}
}

void sound_que::set_velocity(const math::fvec3& velocity)
{
	if (m_velocity != velocity)
	{
		m_velocity = velocity;
		alSourcefv(m_al_source, AL_VELOCITY, m_velocity.data());
	}
}

void sound_que::set_listener_relative(bool relative)
{
	if (m_listener_relative != relative)
	{
		m_listener_relative = relative;
		alSourcei(m_al_source, AL_SOURCE_RELATIVE, m_listener_relative ? AL_TRUE : AL_FALSE);
	}
}

void sound_que::set_reference_distance(float distance)
{
	if (m_reference_distance != distance)
	{
		m_reference_distance = distance;
		alSourcef(m_al_source, AL_REFERENCE_DISTANCE, m_reference_distance);
	}
}

void sound_que::set_rolloff_factor(float factor)
{
	if (m_rolloff_factor != factor)
	{
		m_rolloff_factor = factor;
		alSourcef(m_al_source, AL_ROLLOFF_FACTOR, m_rolloff_factor);
	}
}

void sound_que::set_max_distance(float distance)
{
	if (m_max_distance != distance)
	{
		m_max_distance = distance;
		alSourcef(m_al_source, AL_MAX_DISTANCE, m_max_distance);
	}
}

void sound_que::set_direction(const math::fvec3& direction)
{
	if (m_direction != direction)
	{
		m_direction = direction;
		alSourcefv(m_al_source, AL_DIRECTION, m_direction.data());
	}
}

void sound_que::set_cone_inner_angle(float angle)
{
	if (m_cone_inner_angle != angle)
	{
		m_cone_inner_angle = angle;
		alSourcef(m_al_source, AL_CONE_INNER_ANGLE, math::degrees(m_cone_inner_angle));
	}
}

void sound_que::set_cone_outer_angle(float angle)
{
	if (m_cone_outer_angle != angle)
	{
		m_cone_outer_angle = angle;
		alSourcef(m_al_source, AL_CONE_OUTER_ANGLE, math::degrees(m_cone_outer_angle));
	}
}

void sound_que::set_cone_outer_gain(float gain)
{
	if (m_cone_outer_gain != gain)
	{
		m_cone_outer_gain = gain;
		alSourcef(m_al_source, AL_CONE_OUTER_GAIN, m_cone_outer_gain);
	}
}

void sound_que::set_gain(float gain)
{
	if (m_gain != gain)
	{
		m_gain = gain;
		alSourcef(m_al_source, AL_GAIN, m_gain);
	}
}

void sound_que::set_min_gain(float gain)
{
	if (m_min_gain != gain)
	{
		m_min_gain = gain;
		alSourcef(m_al_source, AL_MIN_GAIN, m_min_gain);
	}
}

void sound_que::set_max_gain(float gain)
{
	if (m_max_gain != gain)
	{
		m_max_gain = gain;
		alSourcef(m_al_source, AL_MAX_GAIN, m_max_gain);
	}
}

void sound_que::set_pitch(float pitch)
{
	if (m_pitch != pitch)
	{
		if (pitch <= 0.0f)
		{
			throw std::out_of_range("Sound que pitch out of range (0, inf].");
		}

		m_pitch = pitch;
		alSourcef(m_al_source, AL_PITCH, m_pitch);
	}
}

void sound_que::set_sound_wave(std::shared_ptr<sound_wave> wave)
{
	if (m_sound_wave == wave)
	{
		return;
	}
	
	// Stop que if currently playing
	if (m_sound_wave && is_playing())
	{
		stop();
	}
	
	if (wave)
	{
		// Attach buffer to source
		alSourcei(m_al_source, AL_BUFFER, wave->m_al_buffer);
		if (auto error = alGetError(); error != AL_NO_ERROR)
		{
			throw std::runtime_error(std::format("OpenAL failed to attach buffer to source: {}", alGetString(error)));
		}
	}
	else
	{
		// Detach buffer from source
		alSourcei(m_al_source, AL_BUFFER, AL_NONE);
		if (auto error = alGetError(); error != AL_NO_ERROR)
		{
			throw std::runtime_error(std::format("OpenAL failed to detach buffer from source: {}", alGetString(error)));
		}
	}
	
	m_sound_wave = std::move(wave);
}

} // namespace audio

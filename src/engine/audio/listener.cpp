// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <AL/al.h>
import engine.audio.listener;

namespace engine::audio
{
	listener::listener()
	{}

	void listener::set_position(const math::fvec3& position)
	{
		if (m_position != position)
		{
			m_position = position;
			alListenerfv(AL_POSITION, m_position.data());
		}
	}

	void listener::set_orientation(const math::fquat& orientation)
	{
		if (m_orientation != orientation)
		{
			m_orientation = orientation;
		
			// Calculate basis vectors
			const math::fvec3 basis[2] =
			{
				// Forward direction vector
				m_orientation * math::fvec3{ 0.0f,  0.0f, -1.0f},
			
				// Up direciton vector
				m_orientation * math::fvec3{ 0.0f,  1.0f,  0.0f}
			};
		
			static_assert(sizeof(basis) == sizeof(float) * 6);
			alListenerfv(AL_ORIENTATION, basis[0].data());
		}
	}

	void listener::set_velocity(const math::fvec3& velocity)
	{
		if (m_velocity != velocity)
		{
			m_velocity = velocity;
			alListenerfv(AL_VELOCITY, m_velocity.data());
		}
	}

	void listener::set_gain(float gain)
	{
		if (m_gain != gain)
		{
			m_gain = gain;
			alListenerf(AL_GAIN, m_gain);
		}
	}
}

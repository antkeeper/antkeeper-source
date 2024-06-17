// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation-track.hpp>
#include <stdexcept>

void animation_track::sample(void* context, float time) const
{
	if (!m_sampler)
	{
		// throw std::runtime_error("Animation track sample failed: no sampler.");
		return;
	}

	if (m_sample_buffer.size() != m_channels.size())
	{
		// Resize sample buffer to accomodate number of channels
		m_sample_buffer.resize(m_channels.size());
	}

	// Sample channels at given time
	for (std::size_t i = 0; i < m_channels.size(); ++i)
	{
		m_sample_buffer[i] = m_channels[i].evaluate(time);
	}

	// Pass sample buffer to sampler function
	m_sampler(context, m_sample_buffer);
}

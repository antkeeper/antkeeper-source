// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/animation-track.hpp>
#include <algorithm>

void animation_track::sample(float time, std::span<float> samples) const
{
	const auto min_size = std::min(m_channels.size(), samples.size());

	for (std::size_t i = 0; i < min_size; ++i)
	{
		samples[i] = m_channels[i].evaluate(time);
	}
}

void animation_track::sample(float time, std::size_t first_channel, std::span<float> samples) const
{
	if (first_channel >= m_channels.size())
	{
		return;
	}

	const auto min_size = std::min(m_channels.size() - first_channel, samples.size());

	for (std::size_t i = 0; i < min_size; ++i)
	{
		samples[i] = m_channels[i + first_channel].evaluate(time);
	}
}

float animation_track::duration() const
{
	float max_duration = 0.0f;
	for (const auto& channel: m_channels)
	{
		max_duration = std::max(max_duration, channel.duration());
	}

	return max_duration;
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.animation.animation_track;
import engine.utility.sized_types;
import engine.math.functions;

namespace engine::animation
{
	void animation_track::sample(float time, std::span<float> samples) const
	{
		const auto min_size = math::min(m_channels.size(), samples.size());

		for (usize i = 0; i < min_size; ++i)
		{
			samples[i] = m_channels[i].evaluate(time);
		}
	}

	void animation_track::sample(float time, usize first_channel, std::span<float> samples) const
	{
		if (first_channel >= m_channels.size())
		{
			return;
		}

		const auto min_size = math::min(m_channels.size() - first_channel, samples.size());

		for (usize i = 0; i < min_size; ++i)
		{
			samples[i] = m_channels[i + first_channel].evaluate(time);
		}
	}

	float animation_track::duration() const
	{
		float max_duration = 0.0f;
		for (const auto& channel : m_channels)
		{
			max_duration = math::max(max_duration, channel.duration());
		}

		return max_duration;
	}
}

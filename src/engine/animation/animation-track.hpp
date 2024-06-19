// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_TRACK_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_TRACK_HPP

#include <engine/animation/animation-curve.hpp>
#include <functional>
#include <span>
#include <vector>

/**
 * Set of related animation channels.
 */
class animation_track
{
public:
	/**
	 * Samples each channel in the track at a given time, passing the evaluated sample data to the sampler function object.
	 *
	 * @param context User-defined animation context.
	 * @param time Time at which to sample the track.
	 */
	void sample(void* context, float time) const;

	/** Returns a reference to the channels of the track. */
	[[nodiscard]] inline constexpr auto& channels() noexcept
	{
		return m_channels;
	}

	/** @copydoc channels() */
	[[nodiscard]] inline constexpr const auto& channels() const noexcept
	{
		return m_channels;
	}

	/**
	 * Returns a reference to the track sampler function object.
	 *
	 * The sampler function object takes two parameters: a void pointer to a user-defined animation context, and a span containing floating-point sample data.
	 */
	[[nodiscard]] inline constexpr auto& sampler() noexcept
	{
		return m_sampler;
	}

	/** @copydoc sampler() */
	[[nodiscard]] inline constexpr const auto& sampler() const noexcept
	{
		return m_sampler;
	}

private:
	std::vector<animation_curve> m_channels;
	std::function<void(void*, std::span<const float>)> m_sampler;
	mutable std::vector<float> m_sample_buffer;
};

#endif // ANTKEEPER_ANIMATION_ANIMATION_TRACK_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_SEQUENCE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_SEQUENCE_HPP

#include <engine/animation/animation-track.hpp>
#include <functional>
#include <map>
#include <string>

/**
 * Set of related animation tracks.
 */
class animation_sequence
{
public:
	/**
	 * Samples all tracks in the animation sequence at a given time.
	 *
	 * @param context User-defined animation context.
	 * @param time Time at which to sample the sequence.
	 */
	void sample_tracks(void* context, float time) const;

	/**
	 * Triggers all cues on the half-open interval [@p start_time, @p end_time).
	 *
	 * @param context User-defined animation context.
	 * @param start_time Start of the interval (inclusive).
	 * @param end_time End of the interval (exclusive).
	 */
	void trigger_cues(void* context, float start_time, float end_time) const;

	/** Returns a reference to the name of the sequence. */
	[[nodiscard]] inline constexpr auto& name() noexcept
	{
		return m_name;
	}

	/** @copydoc name() */
	[[nodiscard]] inline constexpr const auto& name() const noexcept
	{
		return m_name;
	}

	/** Returns a reference the animation tracks of the sequence. */
	[[nodiscard]] inline constexpr auto& tracks() noexcept
	{
		return m_tracks;
	}

	/** @copydoc tracks() */
	[[nodiscard]] inline constexpr const auto& tracks() const noexcept
	{
		return m_tracks;
	}

	/**
	 * Returns a reference to the animation cues of the sequence.
	 *
	 * Cues are function objects mapped to times in the sequence. Cues take one parameter: a void pointer to a user-defined animaton context.
	 */
	[[nodiscard]] inline constexpr auto& cues() noexcept
	{
		return m_cues;
	}

	/** @copydoc cues() */
	[[nodiscard]] inline constexpr const auto& cues() const noexcept
	{
		return m_cues;
	}

private:
	std::string m_name;
	std::map<std::string, animation_track> m_tracks;
	std::multimap<float, std::function<void(void*)>> m_cues;
};

#endif // ANTKEEPER_ANIMATION_SEQUENCE_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_SEQUENCE_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_SEQUENCE_HPP

#include <engine/animation/animation-track.hpp>
#include <map>
#include <string>

/**
 * Set of related animation tracks.
 */
class animation_sequence
{
public:
	/**
	 * Samples the animation sequence at a given time.
	 *
	 * @param context User-defined sampling context.
	 * @param time Time at which to sample the sequence.
	 */
	void sample(void* context, float time) const;

	/** Returns the name of the sequence. */
	[[nodiscard]] inline constexpr auto& name() noexcept
	{
		return m_name;
	}

	/** @copydoc name() */
	[[nodiscard]] inline constexpr const auto& name() const noexcept
	{
		return m_name;
	}

	/** Returns the animation tracks of the sequence. */
	[[nodiscard]] inline constexpr auto& tracks() noexcept
	{
		return m_tracks;
	}

	/** @copydoc tracks() */
	[[nodiscard]] inline constexpr const auto& tracks() const noexcept
	{
		return m_tracks;
	}

private:
	std::string m_name;
	std::map<std::string, animation_track> m_tracks;
};

#endif // ANTKEEPER_ANIMATION_SEQUENCE_HPP

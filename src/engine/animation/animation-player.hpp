// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_ANIMATION_PLAYER_HPP
#define ANTKEEPER_ANIMATION_ANIMATION_PLAYER_HPP

#include <engine/animation/animation-player-state.hpp>
#include <engine/animation/animation-sequence.hpp>
#include <engine/animation/animation-context.hpp>
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <span>
#include <vector>

/**
 * Plays animation sequences.
 */
class animation_player
{
public:
	/**
	 * Advances the animation sequence by a given timestep.
	 *
	 * @param seconds Timestep, in seconds.
	 */
	void advance(float seconds);

	/**
	 * Starts playing an animation sequence.
	 *
	 * @param sequence Animation sequence to play.
	 */
	void play(std::shared_ptr<animation_sequence> sequence);

	/** Starts playing the current animation sequence. */
	void play();
	
	/** Stops playing the current animation sequence. */
	void stop();
	
	/** Rewinds the current animation sequence. */
	void rewind();
	
	/** Pauses the current animation sequence. */
	void pause();

	/**
	 * Sets the playback position of the animation player.
	 *
	 * @param seconds Offset from the start of an animation sequence, in seconds.
	 */
	void seek(float seconds);

	/**
	 * Enables or disables looping of the animation sequence.
	 *
	 * @param enabled `true` to enable looping, `false` to disable looping.
	 */
	void loop(bool enabled);

	/** Returns the state of the animation player. */
	[[nodiscard]] inline constexpr const auto& state() const noexcept
	{
		return m_state;
	}
	
	/** Returns `true` if the animation player is stopped, `false` otherwise. */
	[[nodiscard]] inline constexpr auto is_stopped() const noexcept
	{
		return m_state == animation_player_state::stopped;
	}
	
	/** Returns `true` if the animation player is playing, `false` otherwise. */
	[[nodiscard]] inline constexpr auto is_playing() const noexcept
	{
		return m_state == animation_player_state::playing;
	}
	
	/** Returns `true` if the animation player is paused, `false` otherwise. */
	[[nodiscard]] inline constexpr auto is_paused() const noexcept
	{
		return m_state == animation_player_state::paused;
	}

	/** Returns the playback position of the animation player. */
	[[nodiscard]] inline constexpr auto position() const noexcept
	{
		return m_position;
	}

	/** Returns `true` if sequence looping is enabled, `false` otherwise. */
	[[nodiscard]] inline constexpr auto is_looping() const noexcept
	{
		return m_looping;
	}

	/** Returns a reference to the animation context of the player. */
	[[nodiscard]] inline constexpr auto& context() noexcept
	{
		return m_context;
	}

	/** @copydoc context() */
	[[nodiscard]] inline constexpr const auto& context() const noexcept
	{
		return m_context;
	}

private:
	std::shared_ptr<animation_sequence> m_sequence;
	float m_sequence_duration{};
	animation_player_state m_state{animation_player_state::stopped};
	float m_position{};
	bool m_looping{};
	std::vector<float> m_sample_buffer;
	animation_context m_context{};
};

#endif // ANTKEEPER_ANIMATION_PLAYER_HPP

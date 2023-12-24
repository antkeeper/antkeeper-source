// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AUDIO_LISTENER_HPP
#define ANTKEEPER_AUDIO_LISTENER_HPP

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>

namespace audio {

class sound_system;

/**
 * Sound listener.
 */
class listener
{
public:
	/** Constructs a listener. */
	listener();
	
	/** Destructs a listener. */
	~listener() = default;
	
	listener(const listener&) = delete;
	listener(listener&&) = delete;
	listener& operator=(const listener&) = delete;
	listener& operator=(listener&&) = delete;
	
	/**
	 * Sets the position of the listener.
	 *
	 * @param position Position of the listener.
	 */
	void set_position(const math::fvec3& position);
	
	/**
	 * Sets the orientation of the listener.
	 *
	 * @param orientation Orientation of the listener.
	 */
	void set_orientation(const math::fquat& orientation);
	
	/**
	 * Sets the velocity of the listener.
	 *
	 * @param velocity Position of the listener.
	 */
	void set_velocity(const math::fvec3& velocity);
	
	/**
	 * Sets the gain of the listener.
	 *
	 * @param gain Gain of the listener.
	 */
	void set_gain(float gain);
	
	/** Returns the position of the listener. */
	[[nodiscard]] inline constexpr const auto& get_position() const noexcept
	{
		return m_position;
	}
	
	/** Returns the orientation of the listener. */
	[[nodiscard]] inline constexpr const auto& get_orientation() const noexcept
	{
		return m_orientation;
	}
	
	/** Returns the velocity of the listener. */
	[[nodiscard]] inline constexpr const auto& get_velocity() const noexcept
	{
		return m_velocity;
	}
	
	/** Returns the gain of the listener. */
	[[nodiscard]] inline constexpr auto get_gain() const noexcept
	{
		return m_gain;
	}

private:
	friend class sound_system;
	
	math::fvec3 m_position{};
	math::fquat m_orientation{math::identity<math::fquat>};
	math::fvec3 m_velocity{};
	float m_gain{1.0f};
};

} // namespace audio

#endif // ANTKEEPER_AUDIO_LISTENER_HPP

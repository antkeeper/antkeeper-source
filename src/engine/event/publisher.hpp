// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_EVENT_PUBLISHER_HPP
#define ANTKEEPER_EVENT_PUBLISHER_HPP

#include <engine/event/channel.hpp>

namespace event {

/**
 * Publishes messages to subscribers.
 *
 * @tparam T Message type.
 */
template <class T>
class publisher
{
public:
	/** Message type. */
	using message_type = T;
	
	/** Channel type. */
	using channel_type = channel<message_type>;
	
	/**
	 * Publishes a message.
	 *
	 * @param message Message to publish.
	 */
	void publish(const message_type& message) const
	{
		for (const auto& subscriber: m_channel.m_subscribers)
		{
			(*subscriber)(message);
		}
	}
	
	/** Returns the channel through which messages are published. */
	[[nodiscard]] inline auto& channel() noexcept
	{
		return m_channel;
	}

	/** @copydoc channel() */
	[[nodiscard]] inline const auto& channel() const noexcept
	{
		return m_channel;
	}
	
private:
	channel_type m_channel;
};

} // namespace event

#endif // ANTKEEPER_EVENT_PUBLISHER_HPP

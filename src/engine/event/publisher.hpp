// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_EVENT_PUBLISHER_HPP
#define ANTKEEPER_EVENT_PUBLISHER_HPP

#include <algorithm>
#include <execution>
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
	
	/// @{
	/**
	 * Publishes a message.
	 *
	 * @tparam ExecutionPolicy Execution policy type.
	 *
	 * @param policy Execution policy to use.
	 * @param message Message to publish.
	 */
	template <class ExecutionPolicy>
	void publish(ExecutionPolicy&& policy, const message_type& message) const
	{
		std::for_each
		(
			policy,
			std::begin(m_channel.m_subscribers),
			std::end(m_channel.m_subscribers),
			[&](const auto& subscriber)
			{
				(*subscriber)(message);
			}
		);
	}
	
	void publish(const message_type& message) const
	{
		publish(std::execution::seq, message);
	}
	/// @}
	
	/// @{
	/** Returns the channel through which messages are published. */
	[[nodiscard]] inline const auto& channel() const noexcept
	{
		return m_channel;
	}
	
	[[nodiscard]] inline auto& channel() noexcept
	{
		return m_channel;
	}
	/// @}
	
private:
	channel_type m_channel;
};

} // namespace event

#endif // ANTKEEPER_EVENT_PUBLISHER_HPP

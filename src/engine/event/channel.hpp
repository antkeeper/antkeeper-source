// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/event/subscriber.hpp>
#include <engine/event/subscription.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/event/queue.hpp>
#include <memory>
#include <list>
#include <utility>

namespace engine::event
{
	template <class T>
	class publisher;

	/// Channel through which messages are published.
	/// @tparam T Message type.
	template <class T>
	class channel
	{
	public:
		/// Message type.
		using message_type = T;
	
		/// Subscriber function object type.
		using subscriber_type = subscriber<message_type>;
	
		/// Subscribes a function object to messages published through this channel.
		/// @param subscriber Subscriber function object which will received published messages.
		/// @return Shared subscription object which will unsubscribe the subscriber on destruction.
		[[nodiscard]] std::shared_ptr<subscription> subscribe(subscriber_type&& subscriber)
		{
			// Construct shared pointer to subscriber function
			auto shared_subscriber = std::make_shared<subscriber_type>(std::move(subscriber));
		
			// Append subscriber to subscriber list and store iterator
			auto iterator = m_subscribers.insert(m_subscribers.end(), shared_subscriber);
		
			// Construct and return a shared subscription object which removes the subscriber from the subscriber list when unsubscribed or destructed
			return std::make_shared<subscription>
			(
				std::static_pointer_cast<void>(shared_subscriber),
				[this, iterator = std::move(iterator)]
				{
					this->m_subscribers.erase(iterator);
				}
			);
		}
	
		/// Subscribes a message dispatcher to messages published through this channel.
		/// @param dispatcher Message dispatcher which will receive published messages.
		/// @return Shared subscription object which will unsubscribe the queue on destruction.
		[[nodiscard]] std::shared_ptr<subscription> subscribe(event::dispatcher& dispatcher)
		{
			return subscribe
			(
				[&dispatcher](const message_type& message)
				{
					dispatcher.dispatch<message_type>(message);
				}
			);
		}
	
		/// Subscribes a message queue to messages published through this channel.
		/// @param queue Message queue which will receive published messages.
		/// @return Shared subscription object which will unsubscribe the queue on destruction.
		[[nodiscard]] std::shared_ptr<subscription> subscribe(event::queue& queue)
		{
			return subscribe
			(
				[&queue](const message_type& message)
				{
					queue.enqueue<message_type>(message);
				}
			);
		}
	
	private:
		friend class publisher<T>;
	
		std::list<std::shared_ptr<subscriber_type>> m_subscribers;
	};
}

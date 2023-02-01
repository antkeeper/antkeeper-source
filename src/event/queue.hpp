/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_EVENT_QUEUE_HPP
#define ANTKEEPER_EVENT_QUEUE_HPP

#include <any>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <utility>
#include "event/subscriber.hpp"
#include "event/subscription.hpp"
#include "utility/type-id.hpp"

namespace event {

/**
 * Collects messages from publishers to be distributed to subscribers when desired.
 */
class queue
{
public:
	/**
	 * Subscribes a function object to messages published by this queue.
	 *
	 * @tparam T Message type.
	 *
	 * @param subscriber Function object to subscribe.
	 *
	 * @return Shared subscription object which will unsubscribe the subscriber on destruction.
	 */
	template <class T>
	[[nodiscard]] std::shared_ptr<subscription> subscribe(subscriber<T>&& subscriber)
	{
		// Allocate shared pointer to std::any object containing subscriber
		std::shared_ptr<std::any> shared_subscriber = std::make_shared<std::any>(std::make_any<event::subscriber<T>>(std::move(subscriber)));
		
		// Append subscriber to subscriber list and store iterator
		auto iterator = subscribers.emplace(type_id<T>, shared_subscriber);
		
		// Construct and return a shared subscription object which removes the subscriber from the subscriber list when unsubscribed or destructed
		return std::make_shared<subscription>
		(
			std::static_pointer_cast<void>(shared_subscriber),
			[this, iterator = std::move(iterator)]()
			{
				this->subscribers.erase(iterator);
			}
		);
	}
	
	/**
	 * Adds a message to the queue, to be distributed later.
	 *
	 * @tparam T Message type.
	 *
	 * @param message Message to enqueue.
	 */
	template <class T>
	void enqueue(const T& message)
	{
		messages.emplace_back
		(
			[this, message]()
			{
				this->distribute<T>(message);
			}
		);
	}
	
	/**
	 * Distributes queued messages in FIFO order to subscribers.
	 */
	void flush()
	{
		while (!messages.empty())
		{
			messages.front()();
			messages.pop_front();
		}
	}
	
	/**
	 * Removes all messages from the queue.
	 */
	void clear()
	{
		messages.clear();
	}
	
	/**
	 * Returns `true` if there are no messages in the queue, `false` otherwise.
	 */
	[[nodiscard]] inline bool empty() const noexcept
	{
		return messages.empty();
	}

private:
	/**
	 * Distributes a message.
	 *
	 * @tparam T Message type.
	 *
	 * @param message Message to distribute.
	 */
	template <class T>
	void distribute(const T& message) const
	{
		// For each subscriber of the given message type
		const auto range = subscribers.equal_range(type_id<T>);
		for (auto i = range.first; i != range.second; ++i)
		{
			// Send message to subscriber
			std::any_cast<subscriber<T>>(*(i->second))(message);
		}
	}
	
	std::multimap<type_id_t, std::shared_ptr<std::any>> subscribers;
	std::list<std::function<void()>> messages;
};

} // namespace event

#endif // ANTKEEPER_EVENT_QUEUE_HPP

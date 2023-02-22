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

#include <engine/event/subscriber.hpp>
#include <engine/event/subscription.hpp>
#include <any>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <typeindex>
#include <utility>

namespace event {

/**
 * Collects messages from publishers to be forwarded to subscribers when desired.
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
	 *
	 * @TODO This function should be available through an interface class which does not expose the queue's message-sending functions, such as event::channel for publishers.
	 */
	template <class T>
	[[nodiscard]] std::shared_ptr<subscription> subscribe(subscriber<T>&& subscriber)
	{
		// Allocate shared pointer to std::any object containing subscriber
		std::shared_ptr<std::any> shared_subscriber = std::make_shared<std::any>(std::make_any<event::subscriber<T>>(std::move(subscriber)));
		
		// Append subscriber to subscriber list and store iterator
		auto iterator = subscribers.emplace(std::type_index(typeid(T)), shared_subscriber);
		
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
				this->forward<T>(message);
			}
		);
	}
	
	/**
	 * Forwards queued messages, in FIFO order, to subscribers.
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
	 * Forwards a message to subscribers of the message type.
	 *
	 * @tparam T Message type.
	 *
	 * @param message Message to forward.
	 */
	template <class T>
	void forward(const T& message) const
	{
		// For each subscriber of the given message type
		const auto range = subscribers.equal_range(std::type_index(typeid(T)));
		for (auto i = range.first; i != range.second; ++i)
		{
			// Send message to subscriber
			std::any_cast<subscriber<T>>(*(i->second))(message);
		}
	}
	
	std::multimap<std::type_index, std::shared_ptr<std::any>> subscribers;
	std::list<std::function<void()>> messages;
};

} // namespace event

#endif // ANTKEEPER_EVENT_QUEUE_HPP

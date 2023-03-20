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

#ifndef ANTKEEPER_EVENT_DISPATCHER_HPP
#define ANTKEEPER_EVENT_DISPATCHER_HPP

#include <engine/event/subscriber.hpp>
#include <engine/event/subscription.hpp>
#include <any>
#include <map>
#include <memory>
#include <typeindex>
#include <utility>

namespace event {

/**
 * Forwards messages from publishers to subscribers.
 */
class dispatcher
{
public:
	/**
	 * Subscribes a function object to messages dispatched by this dispatcher.
	 *
	 * @tparam T Message type.
	 *
	 * @param subscriber Function object to subscribe.
	 *
	 * @return Shared subscription object which will unsubscribe the subscriber on destruction.
	 *
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
	 * Dispatches a message to subscribers of the message type.
	 *
	 * @tparam T Message type.
	 *
	 * @param message Message to dispatch.
	 */
	template <class T>
	void dispatch(const T& message) const
	{
		// For each subscriber of the given message type
		const auto range = subscribers.equal_range(std::type_index(typeid(T)));
		for (auto i = range.first; i != range.second; ++i)
		{
			// Send message to subscriber
			std::any_cast<subscriber<T>>(*(i->second))(message);
		}
	}

private:
	std::multimap<std::type_index, std::shared_ptr<std::any>> subscribers;
};

} // namespace event

#endif // ANTKEEPER_EVENT_DISPATCHER_HPP

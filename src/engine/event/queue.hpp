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

#include <engine/event/dispatcher.hpp>
#include <functional>
#include <list>

namespace event {

/**
 * Collects messages from publishers to be dispatched to subscribers when desired.
 */
class queue: public dispatcher
{
public:	
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
				this->dispatch<T>(message);
			}
		);
	}
	
	/**
	 * Dispatches queued messages, in FIFO order, to subscribers.
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
	std::list<std::function<void()>> messages;
};

} // namespace event

#endif // ANTKEEPER_EVENT_QUEUE_HPP

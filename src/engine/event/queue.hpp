// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

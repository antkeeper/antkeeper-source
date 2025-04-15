// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.event.queue;
export import engine.event.dispatcher;
import <list>;
import <functional>;

export namespace engine::event
{
	/// Collects messages from publishers to be dispatched to subscribers when desired.
	class queue: public dispatcher
	{
	public:
		/// Destructs a queue.
		~queue() override = default;
	
		/// Adds a message to the queue, to be distributed later.
		/// @tparam T Message type.
		/// @param message Message to enqueue.
		template <class T>
		void enqueue(const T& message)
		{
			m_messages.emplace_back
			(
				[this, message]()
				{
					this->dispatch<T>(message);
				}
			);
		}
	
		/// Dispatches queued messages, in FIFO order, to subscribers.
		void flush()
		{
			while (!m_messages.empty())
			{
				m_messages.front()();
				m_messages.pop_front();
			}
		}
	
		/// Removes all messages from the queue.
		void clear()
		{
			m_messages.clear();
		}
	
		/// Returns `true` if there are no messages in the queue, `false` otherwise.
		[[nodiscard]] inline bool empty() const noexcept
		{
			return m_messages.empty();
		}

	private:
		std::list<std::function<void()>> m_messages;
	};
}

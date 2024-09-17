// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_EVENT_SUBSCRIPTION_HPP
#define ANTKEEPER_EVENT_SUBSCRIPTION_HPP

#include <functional>
#include <memory>

namespace event {

/**
 * Unsubscribes a subscriber on destruction.
 */
class subscription
{
public:
	/** Unsubscribe function object type. */
	using unsubscribe_type = std::function<void()>;
	
	/**
	 * Constructs a subscription.
	 *
	 * @param subscriber Weak pointer to the subscriber.
	 * @param unsubscriber Unsubscribe function object.
	 * *
	 * @warning @p unsubscriber must be noexcept.
	 */
	subscription(std::weak_ptr<void>&& subscriber, unsubscribe_type&& unsubscriber);
	
	/**
	 * Unsubscribes the subscriber and destructs the subscription.
	 */
	~subscription();
	
	/**
	 * Returns `true` if the subscription is no longer active, `false` otherwise.
	 */
	[[nodiscard]] bool expired() const noexcept;
	
	/**
	 * Unsubscribes the subscriber.
	 */
	void unsubscribe() noexcept;
	
private:
	std::weak_ptr<void> m_subscriber;
	unsubscribe_type m_unsubscriber;
};

/**
 * Shared pointer to a subscription.
 */
using shared_subscription = std::shared_ptr<subscription>;

} // namespace event

#endif // ANTKEEPER_EVENT_SUBSCRIPTION_HPP

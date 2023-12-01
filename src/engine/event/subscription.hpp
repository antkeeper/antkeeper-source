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
	/// Unsubscribe function object type.
	typedef std::function<void()> unsubscribe_type;
	
	/**
	 * Constructs a subscription.
	 *
	 * @param subscriber Weak pointer to the subscriber.
	 * @param unsubscriber Unsubscribe function object.
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
	void unsubscribe();
	
private:
	std::weak_ptr<void> subscriber;
	unsubscribe_type unsubscriber;
};

/**
 * Shared pointer to a subscription.
 */
typedef std::shared_ptr<subscription> shared_subscription;

} // namespace event

#endif // ANTKEEPER_EVENT_SUBSCRIPTION_HPP

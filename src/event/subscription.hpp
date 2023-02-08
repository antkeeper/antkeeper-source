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

} // namespace event

#endif // ANTKEEPER_EVENT_SUBSCRIPTION_HPP

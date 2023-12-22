// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/event/subscription.hpp>
#include <utility>

namespace event {

subscription::subscription(std::weak_ptr<void>&& subscriber, unsubscribe_type&& unsubscriber):
	m_subscriber(std::move(subscriber)),
	m_unsubscriber(std::move(unsubscriber))
{}

subscription::~subscription()
{
	unsubscribe();
}

bool subscription::expired() const noexcept
{
	return m_subscriber.expired();
}

void subscription::unsubscribe()
{
	if (!expired())
	{
		m_unsubscriber();
	}
}

} // namespace event

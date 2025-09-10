// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/event/subscription.hpp>
#include <engine/debug/contract.hpp>
#include <utility>

namespace engine::event
{
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

	void subscription::unsubscribe() noexcept
	{
		if (!expired())
		{
			if (m_unsubscriber)
			{
				m_unsubscriber();
				m_unsubscriber = nullptr;
			}

			m_subscriber.reset();
		}
	}
}

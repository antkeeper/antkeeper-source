// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_EVENT_SUBSCRIBER_HPP
#define ANTKEEPER_EVENT_SUBSCRIBER_HPP

#include <functional>

namespace event {

/**
 * Subscriber function object type.
 *
 * @tparam T Message type.
 */
template <class T>
using subscriber = std::function<void(const T&)>;

} // namespace event

#endif // ANTKEEPER_EVENT_SUBSCRIBER_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.event.subscriber;
export import <functional>;

export namespace engine::event
{
	/// Subscriber function object type.
	/// @tparam T Message type.
	template <class T>
	using subscriber = std::function<void(const T&)>;
}

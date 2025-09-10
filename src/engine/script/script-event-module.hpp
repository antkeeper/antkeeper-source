// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/event/subscription.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/script/script-context.hpp>
#include <string>
#include <unordered_map>
#include <memory>

namespace engine
{
	/// Loads the event module into a script context.
	/// @param ctx Script context.
	/// @param dispatcher Event dispatcher.
	/// @param subscriptions Event subscription map.
	void load_event_module(script_context& ctx, event::dispatcher& dispatcher, std::unordered_map<std::string, std::pair<int, std::shared_ptr<event::subscription>>>& subscriptions);
}

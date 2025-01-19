// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCRIPT_EVENT_MODULE_HPP
#define ANTKEEPER_SCRIPT_EVENT_MODULE_HPP

#include <engine/script/script-context.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/event/subscription.hpp>
#include <string>
#include <memory>
#include <unordered_map>

/// Loads the event module into a script context.
/// @param ctx Script context.
/// @param dispatcher Event dispatcher.
/// @param subscriptions Event subscription map.
auto load_event_module(script_context& ctx, event::dispatcher& dispatcher, std::unordered_map<std::string, std::pair<int, std::shared_ptr<event::subscription>>>& subscriptions) -> void;

#endif // ANTKEEPER_SCRIPT_EVENT_MODULE_HPP

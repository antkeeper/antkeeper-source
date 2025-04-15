// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.script.event_module;
export import engine.event.subscription;
export import engine.event.dispatcher;
import engine.script.context;
export import <string>;
export import <unordered_map>;
export import <memory>;

export namespace engine
{
	/// Loads the event module into a script context.
	/// @param ctx Script context.
	/// @param dispatcher Event dispatcher.
	/// @param subscriptions Event subscription map.
	void load_event_module(script_context& ctx, event::dispatcher& dispatcher, std::unordered_map<std::string, std::pair<int, std::shared_ptr<event::subscription>>>& subscriptions);
}

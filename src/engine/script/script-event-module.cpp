// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <engine/script/script-event-module.hpp>
#include <engine/script/script-context.hpp>
#include <engine/script/script-error.hpp>
#include <engine/input/mouse.hpp>
#include <engine/event/subscription.hpp>
#include <engine/event/dispatcher.hpp>
#include <engine/debug/contract.hpp>
#include <engine/debug/log.hpp>

using namespace engine;

namespace
{
	auto lua_event_to_table(lua_State* lua, const input::mouse_moved_event& e) -> void
	{
		lua_pushnumber(lua, e.position.x());
		lua_setfield(lua, -2, "x");
		lua_pushnumber(lua, e.position.y());
		lua_setfield(lua, -2, "y");
	}

	/// Generates an event subscription object.
	/// @tparam T Event type.
	/// @param dispatcher Event dispatcher
	template <class T>
	[[nodiscard]] std::shared_ptr<event::subscription> lua_event_subscribe_typed(lua_State* lua, int func_ref, event::dispatcher& dispatcher)
	{
		return dispatcher.subscribe<T>
		(
			[lua, func_ref](const auto& event)
			{
				lua_rawgeti(lua, LUA_REGISTRYINDEX, func_ref);

				lua_newtable(lua);
				lua_event_to_table(lua, event);

				if (int status = lua_pcall(lua, 1, 0, 0); status != LUA_OK)
				{
					std::string message = luaL_tolstring(lua, -1, nullptr);
					lua_pop(lua, 1);

					throw script_error(std::move(message), status);
				}

				debug::postcondition(lua_gettop(lua) == 0);
			}
		);
	}

	/// Maps event names to subscription functions
	const std::unordered_map<std::string, std::function<std::shared_ptr<event::subscription>(lua_State*, int, event::dispatcher&)>> event_subscribe_map
	{
		{"mouse_moved", lua_event_subscribe_typed<input::mouse_moved_event>}
	};

	auto lua_event_subscribe(lua_State* lua) -> int
	{
		const auto is_function = lua_isfunction(lua, 2);
		if (!is_function && !lua_isnil(lua, 2))
		{
			return luaL_error(lua, "event: Invalid assignment.");
		}

		std::string event_name = luaL_checkstring(lua, 1);

		auto subscribe_it = event_subscribe_map.find(event_name);
		if (subscribe_it == event_subscribe_map.end())
		{
			return luaL_error(lua, "event: Unknown event type.");
		}

		// Get subscriptions map from upvalues
		auto& subscriptions = *static_cast<std::unordered_map<std::string, std::pair<int, std::shared_ptr<event::subscription>>>*>(lua_touserdata(lua, lua_upvalueindex(2)));

		if (auto subscription_it = subscriptions.find(event_name); subscription_it != subscriptions.end())
		{
			if (is_function)
			{
				// Release reference to old function in Lua registry
				luaL_unref(lua, LUA_REGISTRYINDEX, subscription_it->second.first);
			}
			else
			{
				// nil passed, erase subscription
				subscriptions.erase(subscription_it);

				// Pop nil
				lua_pop(lua, 1);
			}
		}

		if (is_function)
		{
			// Create reference to function (and pop function)
			const int func_ref = luaL_ref(lua, LUA_REGISTRYINDEX);

			// Get dispatcher from upvalues
			auto& dispatcher = *static_cast<event::dispatcher*>(lua_touserdata(lua, lua_upvalueindex(1)));

			// Subscribe function to event
			subscriptions.insert_or_assign(event_name, std::make_pair(func_ref, subscribe_it->second(lua, func_ref, dispatcher)));
		}

		// Pop event name
		lua_pop(lua, 1);

		debug::postcondition(lua_gettop(lua) == 0);
		return 0;
	}

	auto lua_event_get(lua_State* lua) -> int
	{
		std::string event_name = luaL_checkstring(lua, 2);

		auto& subscriptions = *static_cast<std::unordered_map<std::string, std::pair<int, std::shared_ptr<engine::event::subscription>>>*>(lua_touserdata(lua, lua_upvalueindex(1)));

		if (auto subscription_it = subscriptions.find(event_name); subscription_it != subscriptions.end())
		{
			lua_rawgeti(lua, LUA_REGISTRYINDEX, subscription_it->second.first);
		}
		else
		{
			lua_pushnil(lua);
		}

		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}
}

namespace engine
{
	auto load_event_module(script_context& ctx, event::dispatcher& dispatcher, std::unordered_map<std::string, std::pair<int, std::shared_ptr<event::subscription>>>& subscriptions) -> void
	{
		lua_State* lua = ctx.state();

		lua_newtable(lua);

		lua_pushlightuserdata(lua, &dispatcher);
		lua_pushlightuserdata(lua, &subscriptions);
		lua_pushcclosure(lua, lua_event_subscribe, 2);
		lua_setfield(lua, -2, "subscribe");

		lua_setglobal(lua, "event");
	}
}

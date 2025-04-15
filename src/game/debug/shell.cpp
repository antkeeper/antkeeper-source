// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <nlohmann/json.hpp>
#include "game/game.hpp"
#include "game/debug/shell.hpp"
#include "game/strings.hpp"
import engine.config;
import engine.debug.log;
import engine.script.error;
import engine.utility.sized_types;

using namespace engine;

namespace
{
	int lua_get_string(lua_State* L)
	{
		lua_getglobal(L, "ctx");
		game* ctx = static_cast<game*>(lua_touserdata(L, -1));
		lua_pop(L, 1);

		const char* key = luaL_checkstring(L, 2);
		const auto value = get_string(*ctx, key);

		lua_pushlstring(L, value.c_str(), value.length());

		return 1;
	}

	int lua_set_string(lua_State* L)
	{
		lua_getglobal(L, "ctx");
		game* ctx = static_cast<game*>(lua_touserdata(L, -1));
		lua_pop(L, 1);

		const char* key = luaL_checkstring(L, 2);

		usize len = 0;
		const char* value = luaL_tolstring(L, 3, &len);
		
		(*ctx->string_map)[key] = std::string(value, len);

		return 0;
	}

	void register_string(lua_State* L)
	{
		lua_newtable(L);

		luaL_newmetatable(L, "string_metatable");

		lua_pushstring(L, "__index");
		lua_pushcfunction(L, lua_get_string);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, lua_set_string);
		lua_settable(L, -3);

		lua_setmetatable(L, -2);

		lua_setglobal(L, "string");
	}
}

shell::shell(game* ctx):
	m_context{ctx},
	m_output{nullptr}
{
	m_output.clear();

	lua_State* lua = m_context->script.state();

	// Registers globals
	lua_pushlightuserdata(lua, m_context);
	lua_setglobal(lua, "ctx");
	lua_pushstring(lua, config::application_version);
	lua_setglobal(lua, "version");

	register_string(lua);
}

shell::~shell()
{}

int shell::interpret(const std::string_view& input)
{
	// Write input to output
	m_output << prompt() << input << '\n';

	if (!input.empty())
	{
		// Append input to history
		m_history.emplace_back(input);

		// Execute input
		try
		{
			try
			{
				// Attempt to execute with "return " prefixed to input string
				m_context->script.execute(std::format("return {};", input));
				m_status = 0;
			}
			catch (const script_error&)
			{
				// return-prefixed execution failed, execute unmodified input string
				m_context->script.execute(input);
				m_status = 0;
			}
			
			lua_State* lua = m_context->script.state();

			// Print stack
			const auto n = lua_gettop(lua);
			for (int i = 1; i <= n; ++i)
			{
				usize len = 0;
				const char* str = luaL_tolstring(lua, i, &len);

				if (!str)
				{
					throw std::runtime_error("Failed to print stack item.");
				}

				m_output << std::format("{}\n", std::string_view(str, len));

				lua_pop(lua, 1);
			}

			// Clear stack
			lua_settop(lua, 0);
		}
		catch (const script_error& e)
		{
			m_output << std::format("Error: {}\n", e.what());
			m_status = e.code();
		}
	}

	// Flush output buffer
	m_output << std::flush;
	
	return m_status;
}

std::string shell::prompt()
{
	return m_status ? std::format("[{}]> ", m_status) : "> ";
}

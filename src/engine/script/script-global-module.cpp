// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

import engine.script.context;
import engine.script.global_module;
import engine.debug.contract;

using namespace engine;

namespace
{
	auto lua_global_help(lua_State* lua) -> int
	{
		lua_pushglobaltable(lua);

		lua_pushnil(lua);
		while (lua_next(lua, -2) != 0)
		{
			if (lua_isfunction(lua, -1))
			{
				lua_getglobal(lua, "io");
				lua_getfield(lua, -1, "println");

				// Push function name
				lua_pushvalue(lua, 2);

				if (lua_pcall(lua, 1, 0, 0) != LUA_OK)
				{
					// Pop error message
					lua_pop(lua, 1);
				}

				// Pop io table
				lua_pop(lua, 1);
			}

			// Pop value
			lua_pop(lua, 1);
		}

		// Pop global table
		lua_pop(lua, 1);

		debug::postcondition(lua_gettop(lua) == 0);
		return 0;
	}
}

namespace engine
{
	auto load_global_module(script_context& ctx) -> void
	{
		lua_State* lua = ctx.state();

		lua_pushcclosure(lua, lua_global_help, 0);
		lua_setglobal(lua, "help");
	}
}

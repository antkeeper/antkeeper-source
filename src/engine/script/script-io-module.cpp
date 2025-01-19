// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/script/script-io-module.hpp>
#include <engine/script/script-context.hpp>
#include <engine/debug/contract.hpp>
#include <string_view>

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

namespace
{
	auto lua_io_print(lua_State* lua) -> int
	{
		std::ostream& cout = *static_cast<std::ostream*>(lua_touserdata(lua, lua_upvalueindex(1)));

		const int n = lua_gettop(lua);
		for (int i = 1; i <= n; ++i)
		{
			std::size_t len = 0;
			const char* str = luaL_tolstring(lua, i, &len);
			cout << std::string_view(str, len);

			// Pop string
			lua_pop(lua, 1);
		}

		// Pop arguments
		lua_pop(lua, n);

		debug::postcondition(lua_gettop(lua) == 0);
		return 0;
	}

	auto lua_io_println(lua_State* lua) -> int
	{
		// Warning: calling lua_io_print here means it will actually be accessing this function's upvalues
		lua_io_print(lua);

		std::ostream& cout = *static_cast<std::ostream*>(lua_touserdata(lua, lua_upvalueindex(1)));
		cout << '\n';

		debug::postcondition(lua_gettop(lua) == 0);
		return 0;
	}

	auto lua_io_flush(lua_State* lua) -> int
	{
		std::ostream& cout = *static_cast<std::ostream*>(lua_touserdata(lua, lua_upvalueindex(1)));
		cout << std::flush;

		debug::postcondition(lua_gettop(lua) == 0);
		return 0;
	}
}

auto load_io_module(script_context& ctx, std::ostream& cout) -> void
{
	lua_State* lua = ctx.state();

	lua_newtable(lua);

	lua_pushlightuserdata(lua, &cout);
	lua_pushcclosure(lua, lua_io_print, 1);
	lua_setfield(lua, -2, "print");

	lua_pushlightuserdata(lua, &cout);
	lua_pushcclosure(lua, lua_io_println, 1);
	lua_setfield(lua, -2, "println");

	lua_pushlightuserdata(lua, &cout);
	lua_pushcclosure(lua, lua_io_flush, 1);
	lua_setfield(lua, -2, "flush");

	lua_setglobal(lua, "io");
}

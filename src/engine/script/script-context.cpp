// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/script/script-context.hpp>
#include <engine/script/script-error.hpp>
#include <string>
#include <utility>

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

script_context::script_context()
{
	m_state = luaL_newstate();
}

script_context::script_context(script_context&& other) noexcept
{
	std::swap(m_state, other.m_state);
}

script_context::~script_context()
{
	if (m_state)
	{
		lua_close(m_state);
	}
}

auto script_context::operator=(script_context&& other) noexcept -> script_context&
{
	lua_close(m_state);
	m_state = nullptr;
	std::swap(m_state, other.m_state);

	return *this;
}

auto script_context::execute(std::string_view input) -> void
{
	int status =
		luaL_loadbuffer(m_state, input.data(), input.length(), input.data()) ||
		lua_pcall(m_state, 0, LUA_MULTRET, 0);

	if (status != LUA_OK)
	{
		std::string message = lua_tostring(m_state, -1);
		lua_pop(m_state, 1);

		throw script_error(std::move(message), status);
	}
}

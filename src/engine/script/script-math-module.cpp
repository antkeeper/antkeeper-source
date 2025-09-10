// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <engine/script/script-context.hpp>
#include <engine/script/script-math-module.hpp>
#include <engine/debug/contract.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/constants.hpp>
#include <engine/math/functions.hpp>

using namespace engine;

namespace
{
	auto lua_math_min(lua_State* lua) -> int
	{
		const auto result = math::min(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
		lua_pop(lua, 2);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_max(lua_State* lua) -> int
	{
		const auto result = math::max(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
		lua_pop(lua, 2);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_clamp(lua_State* lua) -> int
	{
		const auto result = math::clamp(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3));
		lua_pop(lua, 3);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_abs(lua_State* lua) -> int
	{
		const auto result = math::abs(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_ceil(lua_State* lua) -> int
	{
		const auto result = math::ceil(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_floor(lua_State* lua) -> int
	{
		const auto result = math::floor(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_degrees(lua_State* lua) -> int
	{
		const auto result = math::degrees(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_radians(lua_State* lua) -> int
	{
		const auto result = math::radians(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_wrap_degrees(lua_State* lua) -> int
	{
		const auto result = math::wrap_degrees(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_wrap_radians(lua_State* lua) -> int
	{
		const auto result = math::wrap_radians(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_exp(lua_State* lua) -> int
	{
		const auto result = math::exp(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_exp2(lua_State* lua) -> int
	{
		const auto result = math::exp2(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_log(lua_State* lua) -> int
	{
		const auto result = math::log(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_log2(lua_State* lua) -> int
	{
		const auto result = math::log2(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_pow(lua_State* lua) -> int
	{
		const auto result = math::pow(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
		lua_pop(lua, 2);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_sqr(lua_State* lua) -> int
	{
		const auto result = math::sqr(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_cube(lua_State* lua) -> int
	{
		const auto result = math::cube(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_sqrt(lua_State* lua) -> int
	{
		const auto result = math::sqrt(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_inversesqrt(lua_State* lua) -> int
	{
		const auto result = math::inversesqrt(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_sin(lua_State* lua) -> int
	{
		const auto result = math::sin(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_cos(lua_State* lua) -> int
	{
		const auto result = math::cos(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_tan(lua_State* lua) -> int
	{
		const auto result = math::tan(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_asin(lua_State* lua) -> int
	{
		const auto result = math::asin(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_acos(lua_State* lua) -> int
	{
		const auto result = math::acos(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_atan(lua_State* lua) -> int
	{
		lua_Number result{};

		const auto n = lua_gettop(lua);
		if (n == 2)
		{
			result = math::atan(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
		}
		else if (n == 1)
		{
			result = math::atan(luaL_checknumber(lua, 1));
		}
		else
		{
			return luaL_error(lua, "atan: Expected 1 or 2 arguments.");
		}

		lua_pop(lua, n);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_fract(lua_State* lua) -> int
	{
		const auto result = math::fract(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_trunc(lua_State* lua) -> int
	{
		const auto result = math::trunc(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_sign(lua_State* lua) -> int
	{
		const auto result = math::sign(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_mod(lua_State* lua) -> int
	{
		const auto result = math::mod(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2));
		lua_pop(lua, 2);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_modf(lua_State* lua) -> int
	{
		const auto [f, i] = math::modf(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushnumber(lua, f);
		lua_pushnumber(lua, i);
		debug::postcondition(lua_gettop(lua) == 2);
		return 2;
	}

	auto lua_math_lerp(lua_State* lua) -> int
	{
		const auto result = math::lerp(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3));
		lua_pop(lua, 3);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_lerp_angle(lua_State* lua) -> int
	{
		const auto result = math::lerp_angle(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3));
		lua_pop(lua, 3);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_log_lerp(lua_State* lua) -> int
	{
		const auto result = math::log_lerp(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3));
		lua_pop(lua, 3);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_isnan(lua_State* lua) -> int
	{
		const auto result = math::isnan(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushboolean(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_isinf(lua_State* lua) -> int
	{
		const auto result = math::isinf(luaL_checknumber(lua, 1));
		lua_pop(lua, 1);
		lua_pushboolean(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}

	auto lua_math_map(lua_State* lua) -> int
	{
		const auto result = math::map_range(luaL_checknumber(lua, 1), luaL_checknumber(lua, 2), luaL_checknumber(lua, 3), luaL_checknumber(lua, 4), luaL_checknumber(lua, 5));
		lua_pop(lua, 5);
		lua_pushnumber(lua, result);
		debug::postcondition(lua_gettop(lua) == 1);
		return 1;
	}
}

namespace engine
{
	auto load_math_module(script_context& ctx) -> void
	{
		lua_State* lua = ctx.state();

		lua_newtable(lua);

		// Constants
		lua_pushnumber(lua, math::inf<double>);
		lua_setfield(lua, -2, "inf");
		lua_pushnumber(lua, math::e<double>);
		lua_setfield(lua, -2, "e");
		lua_pushnumber(lua, math::pi<double>);
		lua_setfield(lua, -2, "pi");
		lua_pushnumber(lua, math::phi<double>);
		lua_setfield(lua, -2, "phi");

		// Basic operations
		lua_pushcfunction(lua, lua_math_min);
		lua_setfield(lua, -2, "min");
		lua_pushcfunction(lua, lua_math_max);
		lua_setfield(lua, -2, "max");
		lua_pushcfunction(lua, lua_math_clamp);
		lua_setfield(lua, -2, "clamp");
		lua_pushcfunction(lua, lua_math_abs);
		lua_setfield(lua, -2, "abs");
		lua_pushcfunction(lua, lua_math_ceil);
		lua_setfield(lua, -2, "ceil");
		lua_pushcfunction(lua, lua_math_floor);
		lua_setfield(lua, -2, "floor");

		// Angular conversion
		lua_pushcfunction(lua, lua_math_degrees);
		lua_setfield(lua, -2, "degrees");
		lua_pushcfunction(lua, lua_math_radians);
		lua_setfield(lua, -2, "radians");
		lua_pushcfunction(lua, lua_math_wrap_degrees);
		lua_setfield(lua, -2, "wrap_degrees");
		lua_pushcfunction(lua, lua_math_wrap_radians);
		lua_setfield(lua, -2, "wrap_radians");

		// Exponential functions
		lua_pushcfunction(lua, lua_math_exp);
		lua_setfield(lua, -2, "exp");
		lua_pushcfunction(lua, lua_math_exp2);
		lua_setfield(lua, -2, "exp2");
		lua_pushcfunction(lua, lua_math_log);
		lua_setfield(lua, -2, "log");
		lua_pushcfunction(lua, lua_math_log2);
		lua_setfield(lua, -2, "log2");

		// Power functions
		lua_pushcfunction(lua, lua_math_pow);
		lua_setfield(lua, -2, "pow");
		lua_pushcfunction(lua, lua_math_sqr);
		lua_setfield(lua, -2, "sqr");
		lua_pushcfunction(lua, lua_math_cube);
		lua_setfield(lua, -2, "cube");
		lua_pushcfunction(lua, lua_math_sqrt);
		lua_setfield(lua, -2, "sqrt");
		lua_pushcfunction(lua, lua_math_inversesqrt);
		lua_setfield(lua, -2, "inversesqrt");

		// Trigonometric functions
		lua_pushcfunction(lua, lua_math_sin);
		lua_setfield(lua, -2, "sin");
		lua_pushcfunction(lua, lua_math_cos);
		lua_setfield(lua, -2, "cos");
		lua_pushcfunction(lua, lua_math_tan);
		lua_setfield(lua, -2, "tan");
		lua_pushcfunction(lua, lua_math_asin);
		lua_setfield(lua, -2, "asin");
		lua_pushcfunction(lua, lua_math_acos);
		lua_setfield(lua, -2, "acos");
		lua_pushcfunction(lua, lua_math_atan);
		lua_setfield(lua, -2, "atan");

		// Floating-point manipulation functions
		lua_pushcfunction(lua, lua_math_fract);
		lua_setfield(lua, -2, "fract");
		lua_pushcfunction(lua, lua_math_trunc);
		lua_setfield(lua, -2, "trunc");
		lua_pushcfunction(lua, lua_math_sign);
		lua_setfield(lua, -2, "sign");
		lua_pushcfunction(lua, lua_math_mod);
		lua_setfield(lua, -2, "mod");
		lua_pushcfunction(lua, lua_math_modf);
		lua_setfield(lua, -2, "modf");

		// Interpolation functions
		lua_pushcfunction(lua, lua_math_lerp);
		lua_setfield(lua, -2, "lerp");
		lua_pushcfunction(lua, lua_math_lerp_angle);
		lua_setfield(lua, -2, "lerp_angle");
		lua_pushcfunction(lua, lua_math_log_lerp);
		lua_setfield(lua, -2, "log_lerp");

		// Classification and comparison 
		lua_pushcfunction(lua, lua_math_isnan);
		lua_setfield(lua, -2, "isnan");
		lua_pushcfunction(lua, lua_math_isinf);
		lua_setfield(lua, -2, "isinf");

		// Utility functions
		lua_pushcfunction(lua, lua_math_map);
		lua_setfield(lua, -2, "map");

		lua_setglobal(lua, "math");
	}
}

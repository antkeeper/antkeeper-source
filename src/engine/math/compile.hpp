// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>

namespace engine::math::compile
{
	/// Compile-time `ceil(log2(x))` for unsigned integrals.
	/// @param x Input value.
	/// @return `ceil(log2(x))`.
	template <std::unsigned_integral T>
	[[nodiscard]] consteval T ceil_log2(T x)
	{
		return (x <= T(1)) ? T(0) : ceil_log2((x + T(1)) / T(2)) + T(1);
	}

	/// Compile-time `exp2` for unsigned integrals.
	/// @param x Input value.
	/// @return `exp2(x)`.
	template <std::unsigned_integral T>
	[[nodiscard]] consteval T exp2(T x)
	{
		return (x) ? T(2) << (x - 1) : T(1);
	}

	/// Compile-time `pow` for unsigned integrals.
	/// @param x Base value.
	/// @param e Integral exponent.
	/// @return `x^e`.
	template <std::unsigned_integral T>
	[[nodiscard]] consteval T pow(T x, T e)
	{
		return (e == 0) ? T(1) : (x * pow<T>(x, e - 1));
	}
}

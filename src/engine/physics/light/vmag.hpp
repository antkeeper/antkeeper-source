// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/functions.hpp>

namespace engine::physics::light
{
	/// Converts apparent magnitude to a brightness factor relative to a 0th magnitude star.
	/// @param mv Apparent magnitude.
	/// @return Brightness factor relative to a 0th magnitude star.
	/// @see https://en.wikipedia.org/wiki/Illuminance
	template <class T>
	[[nodiscard]] T vmag_to_brightness(T mv)
	{
		// 100^(1/5)
		constexpr T fifth_root_100 = T{2.5118864315095801110850320677993};
		return math::pow(fifth_root_100, -mv);
	}

	/// Converts apparent magnitude to illuminance.
	/// @param mv Apparent magnitude.
	/// @return Illuminance, in lux.
	/// @see https://en.wikipedia.org/wiki/Illuminance
	template <class T>
	[[nodiscard]] T vmag_to_illuminance(T mv)
	{
		return math::pow(T{10}, (T{-14.18} - mv) * T{0.4});
	}

	/// Converts illuminance to apparent magnitude.
	/// @param ev Illuminance, in lux.
	/// @return Apparent magnitude.
	/// @see https://en.wikipedia.org/wiki/Illuminance
	template <class T>
	[[nodiscard]] T illuminance_to_vmag(T ev)
	{
		return T{-14.18} - T{2.5} * math::log10(ev);
	}
}

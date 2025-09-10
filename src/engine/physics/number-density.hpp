// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/physics/constants.hpp>

namespace engine::physics
{
	/// Calculates the number density of a substance.
	/// @param c Molar concentration, in mol/m-3.
	/// @return Number density, in m-3.
	/// @see https://en.wikipedia.org/wiki/Number_density
	template <class T>
	[[nodiscard]] inline constexpr T number_density(T c) noexcept
	{
		return physics::constants::avogadro<T> *c;
	}
}

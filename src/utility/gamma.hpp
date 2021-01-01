/*
 * Copyright (C) 2021  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ANTKEEPER_GAMMA_HPP
#define ANTKEEPER_GAMMA_HPP

#include "math/vector-type.hpp"
#include <cmath>

template <typename T>
T srgb_to_linear(const T& x)
{
	if (x <= T(0.04045))
	{
		return x / T(12.92);
	}
	
	return std::pow((x + T(0.055)) / T(1.055), T(2.4));
}

template <typename T>
T linear_to_srgb(const T& x)
{
	if (x <= T(0.0031308))
	{
		return x * T(12.92);
	}
	
	return std::pow(x, T(1.0 / 2.4)) * T(1.055) - T(0.055);
}

#endif // ANTKEEPER_GAMMA_HPP

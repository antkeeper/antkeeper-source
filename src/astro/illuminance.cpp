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

#include "illuminance.hpp"
#include <cmath>

namespace astro
{

double vmag_to_brightness(double mv)
{
	// 100^(1/5)
	static constexpr double fifth_root_100 = 2.5118864315095801110850320677993;
	return std::pow(fifth_root_100, -mv);
}

double vmag_to_lux(double mv)
{
	return std::pow(10.0, (-14.18 - mv) * 0.4);
}

double lux_to_vmag(double ev)
{
	return -14.18 - 2.5 * std::log10(ev);
}

} // namespace astro

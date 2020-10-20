/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_BLACKBODY_HPP
#define ANTKEEPER_BLACKBODY_HPP

#include "utility/fundamental-types.hpp"

namespace ast
{

/**
 * Calculates the color of an incandescent black-body.
 *
 * @param t Temperature, in Kelvin.
 * @param l Luminance value.
 * @return Linear RGB color.
 *
 * @see https://en.wikipedia.org/wiki/Planckian_locus
 * @see https://en.wikipedia.org/wiki/CIE_1960_color_space
 */
double3 blackbody(double t, double l);

} // namespace ast

#endif // ANTKEEPER_BLACKBODY_HPP

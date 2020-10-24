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

#ifndef ANTKEEPER_COLOR_INDEX_HPP
#define ANTKEEPER_COLOR_INDEX_HPP

namespace ast
{

/**
 * Approximates the temperature of a star, given its B-V index.
 *
 * @see Ballesteros, F. J. (2012). "New insights into black bodies". EPL 97 (2012) 34008.
 *
 * @param bv B-V index.
 * @return Temperature, in Kelvin.
 */
double bv_to_k(double bv);

} // namespace ast

#endif // ANTKEEPER_COLOR_INDEX_HPP

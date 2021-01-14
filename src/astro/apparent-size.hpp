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

#ifndef ANTKEEPER_ASTRO_APPARENT_SIZE_HPP
#define ANTKEEPER_ASTRO_APPARENT_SIZE_HPP

namespace astro
{

/**
 * Finds the angular radius of a celestial object, given its radius and distance.
 *
 * @param radius Radius of the celestial object.
 * @param distance Distance to the celestial object.
 * @return Angular radius, in radians.
 */
double find_angular_radius(double radius, double distance);

} // namespace astro

#endif // ANTKEEPER_ASTRO_APPARENT_SIZE_HPP

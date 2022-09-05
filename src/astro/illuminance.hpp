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

#ifndef ANTKEEPER_ASTRO_ILLUMINANCE_HPP
#define ANTKEEPER_ASTRO_ILLUMINANCE_HPP

namespace astro
{

/**
 * Converts apparent (visual) magnitude to a brightness factor relative to a 0th magnitude star.
 *
 * @param mv Illuminance in apparent magnitude.
 * @return Relative brightness factor.
 *
 * @see https://en.wikipedia.org/wiki/Illuminance
 */
double vmag_to_brightness(double mv);

/**
 * Converts apparent (visual) magnitude to lux.
 *
 * @param mv Illuminance in apparent magnitude.
 * @return Illuminance in lux.
 *
 * @see https://en.wikipedia.org/wiki/Illuminance
 */
double vmag_to_lux(double mv);

/**
 * Converts lux to apparent (visual) magnitude.
 *
 * @param ev Illuminance in lux.
 * @return Illuminance in apparent magnitude.
 *
 * @see https://en.wikipedia.org/wiki/Illuminance
 */
double lux_to_vmag(double ev);

} // namespace astro

#endif // ANTKEEPER_ASTRO_ILLUMINANCE_HPP

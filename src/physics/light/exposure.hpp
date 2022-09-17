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

#ifndef ANTKEEPER_PHYSICS_LIGHT_EV_HPP
#define ANTKEEPER_PHYSICS_LIGHT_EV_HPP

#include <cmath>

namespace physics {
namespace light {

/**
 * Exposure value.
 *
 * @see https://en.wikipedia.org/wiki/Exposure_value
 */
namespace ev {

/**
 * Exposure value from luminance.
 *
 * @param l Luminance, in cd/m^2.
 * @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
 * @param k Reflected-light meter calibration constant. A common value is `12.5`.
 *
 * @return Exposure value.
 */
template <class T>
T from_luminance(T l, T s, T k)
{
	return std::log2((l * s) / k);
}

/**
 * Exposure value from illuminance.
 *
 * @param e Illuminance, in lux.
 * @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
 * @param c Incident-light meter calibration constant. A common value is `250`.
 *
 * @return Exposure value.
 */
template <class T>
T from_illuminance(T e, T s, T c)
{
	return std::log2((e * s) / c);
}

/**
 * Exposure value from exposure settings.
 *
 * @param n Relative aperture (f-number).
 * @param t Exposure time (shutter speed), in seconds.
 * @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
 *
 * @return Exposure value.
 */
template <class T>
T from_settings(T n, T t, T s)
{
	return std::log2((n * n) / t * T(100) / s);
}

/**
 * Exposure value to luminance.
 *
 * @param ev Exposure value.
 * @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
 * @param k Reflected-light meter calibration constant. A common value is `12.5`.
 *
 * @return Luminance, in cd/m^2.
 */
template <class T>
T to_luminance(T ev, T s, T k)
{
	return  (k * std::exp2(ev)) / s;
}

/**
 * Exposure value to luminance.
 *
 * @param ev Exposure value.
 * @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
 * @param k Incident-light meter calibration constant. A common value is `250`.
 *
 * @return Illuminance, in lux.
 */
template <class T>
T to_luminance(T ev, T s, T c)
{
	return  (c * std::exp2(ev)) / s;
}

} // namespace ev
} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_EV_HPP

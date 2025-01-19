// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_LIGHT_EV_HPP
#define ANTKEEPER_PHYSICS_LIGHT_EV_HPP

#include <cmath>

namespace physics {
namespace light {

/// Exposure value.
/// @see https://en.wikipedia.org/wiki/Exposure_value
namespace ev {

/// Exposure value from luminance.
/// @param l Luminance, in cd/m^2.
/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
/// @param k Reflected-light meter calibration constant. A common value is `12.5`.
/// @return Exposure value.
template <class T>
T from_luminance(T l, T s, T k)
{
	return std::log2((l * s) / k);
}

/// Exposure value from illuminance.
/// @param e Illuminance, in lux.
/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
/// @param c Incident-light meter calibration constant. A common value is `250`.
/// @return Exposure value.
template <class T>
T from_illuminance(T e, T s, T c)
{
	return std::log2((e * s) / c);
}

/// Exposure value from exposure settings.
/// @param n Relative aperture (f-number).
/// @param t Exposure time (shutter speed), in seconds.
/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
/// @return Exposure value.
template <class T>
T from_settings(T n, T t, T s)
{
	return std::log2((n * n) / t * T(100) / s);
}

/// Exposure value to luminance.
/// @param ev Exposure value.
/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
/// @param k Reflected-light meter calibration constant. A common value is `12.5`.
/// @return Luminance, in cd/m^2.
template <class T>
T to_luminance(T ev, T s, T k)
{
	return  (k * std::exp2(ev)) / s;
}

/// Exposure value to illuminance.
/// @param ev Exposure value.
/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
/// @param c Incident-light meter calibration constant. A common value is `250`.
/// @return Illuminance, in lux.
template <class T>
T to_illuminance(T ev, T s, T c)
{
	return  (c * std::exp2(ev)) / s;
}

} // namespace ev
} // namespace light
} // namespace physics

#endif // ANTKEEPER_PHYSICS_LIGHT_EV_HPP

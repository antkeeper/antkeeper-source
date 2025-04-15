// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.light.exposure;
import engine.math.functions;

export namespace engine::physics::light
{
	/// Exposure value from luminance.
	/// @param l Luminance, in cd/m^2.
	/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
	/// @param k Reflected-light meter calibration constant. A common value is `12.5`.
	/// @return Exposure value.
	template <class T>
	[[nodiscard]] T luminance_to_ev(T l, T s, T k)
	{
		return math::log2((l * s) / k);
	}

	/// Exposure value from illuminance.
	/// @param e Illuminance, in lux.
	/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
	/// @param c Incident-light meter calibration constant. A common value is `250`.
	/// @return Exposure value.
	template <class T>
	[[nodiscard]] T illuminance_to_ev(T e, T s, T c)
	{
		return math::log2((e * s) / c);
	}

	/// Exposure value from exposure settings.
	/// @param n Relative aperture (f-number).
	/// @param t Exposure time (shutter speed), in seconds.
	/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
	/// @return Exposure value.
	template <class T>
	[[nodiscard]] T settings_to_ev(T n, T t, T s)
	{
		return math::log2((n * n) / t * T(100) / s);
	}

	/// Exposure value to luminance.
	/// @param ev Exposure value.
	/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
	/// @param k Reflected-light meter calibration constant. A common value is `12.5`.
	/// @return Luminance, in cd/m^2.
	template <class T>
	[[nodiscard]] T ev_to_luminance(T ev, T s, T k)
	{
		return  (k * math::exp2(ev)) / s;
	}

	/// Exposure value to illuminance.
	/// @param ev Exposure value.
	/// @param s ISO arithmetic speed. A value of `100` corresponds to ISO 100.
	/// @param c Incident-light meter calibration constant. A common value is `250`.
	/// @return Illuminance, in lux.
	template <class T>
	[[nodiscard]] T ev_to_illuminance(T ev, T s, T c)
	{
		return  (c * math::exp2(ev)) / s;
	}
}

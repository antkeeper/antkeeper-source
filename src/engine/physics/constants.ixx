// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.constants;

/// Physical constants
export namespace engine::physics::constants
{
	/// Avogadro number (N).
	/// @see https://physics.nist.gov/cgi-bin/cuu/Value?na
	template <class T>
	inline constexpr T avogadro = T{6.02214076e+23};

	/// Boltzmann constant (k), in joule per kelvin.
	/// @see https://physics.nist.gov/cgi-bin/cuu/Value?k
	template <class T>
	inline constexpr T boltzmann = T{1.380649e-23};

	/// Molar gas constant (R), in joule per kelvin per mole.
	/// @see https://en.wikipedia.org/wiki/Gas_constant
	template <class T>
	inline constexpr T gas = T{8.31446261815324};

	/// Gravitational constant (G), in cubic meter per second squared per kilogram.
	/// @see https://physics.nist.gov/cgi-bin/cuu/Value?G
	template <class T>
	inline constexpr T gravitational = T{6.67430e-11};

	/// Planck constant (h), in joule per hertz.
	/// @see https://physics.nist.gov/cgi-bin/cuu/Value?h
	template <class T>
	inline constexpr T planck = T{6.62607015e-34};

	/// Stefan-Boltzmann constant (sigma), in watt per square meter kelvin to the fourth.
	/// @see https://en.wikipedia.org/wiki/Stefan%E2%80%93Boltzmann_constant
	template <class T>
	inline constexpr T stefan_boltzmann = T{5.67037441918442945397099673188923087584012297029130e-8};

	/// Speed of light in vacuum (c), in meters per second.
	/// @see https://physics.nist.gov/cgi-bin/cuu/Value?c
	template <class T>
	inline constexpr T speed_of_light = T{299792458};
}

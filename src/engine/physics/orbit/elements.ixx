// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.orbit.elements;
import engine.math.functions;
import engine.math.vector;
import engine.math.constants;

export namespace engine::physics::orbit
{
	/// Set of six Keplerian elements required to uniquely identify an orbit.
	/// @tparam T Scalar type.
	template <class T>
	struct elements
	{
		/// Scalar type.
		using scalar_type = T;

		/// Eccentricity (e).
		scalar_type ec;

		/// Semimajor axis (a).
		scalar_type a;

		/// Inclination (i), in radians.
		scalar_type in;

		/// Right ascension of the ascending node (OMEGA), in radians.
		scalar_type om;

		/// Argument of periapsis (omega), in radians.
		scalar_type w;

		/// Mean anomaly (M) at epoch, in radians.
		scalar_type ma;
	};

	/// Calculates the mean motion (n) of an orbit.
	/// @param a Semimajor axis (a).
	/// @param gm Standard gravitational parameter (GM).
	/// @return Mean motion (n), in radians per unit time.
	template <class T>
	[[nodiscard]] T mean_motion(T a, T gm)
	{
		return math::sqrt((a * a * a) / gm);
	}

	/// Calculates the period of an elliptical orbit according to Kepler's third law.
	/// @param a Semimajor axis (a).
	/// @param gm Standard gravitational parameter (GM).
	/// @return Orbital period (T).
	template <class T>
	[[nodiscard]] T period(T a, T gm)
	{
		return math::two_pi<T> *mean_motion(a, gm);
	}

	/// Calculates the mean motion (n) of an orbit.
	/// @param t Orbital period (T).
	/// @return Mean motion (n), in radians per unit time.
	template <class T>
	[[nodiscard]] constexpr T mean_motion(T t) noexcept
	{
		return math::two_pi<T> / t;
	}

	/// Derives the argument of the periapsis (omega) of an orbit, given the longitude of periapsis (pomega) and longitude of the ascending node (OMEGA).
	/// @param lp Longitude of the periapsis (pomega), in radians.
	/// @param om Right ascension of the ascending node (OMEGA), in radians.
	/// @return Argument of the periapsis (omega), in radians.
	template <class T>
	[[nodiscard]] constexpr T argument_periapsis(T om, T lp) noexcept
	{
		return lp - om;
	}

	/// Derives the longitude of the periapsis (pomega) of an orbit, given the argument of periapsis (omega) and longitude of the ascending node (OMEGA).
	/// @param w Argument of the periapsis (omega), in radians.
	/// @param om Right ascension of the ascending node (OMEGA), in radians.
	/// @return Longitude of the periapsis (pomega), in radians.
	template <class T>
	[[nodiscard]] constexpr T longitude_periapsis(T om, T w) noexcept
	{
		return w + om;
	}

	/// Derives the semiminor axis (b) of an orbit, given the semimajor axis (a) and eccentricity (e).
	/// @param a Semimajor axis (a).
	/// @param ec Eccentricity (e).
	/// @return Semiminor axis (b).
	template <class T>
	[[nodiscard]] T semiminor_axis(T a, T ec)
	{
		return a * math::sqrt(T{1} - ec * ec);
	}

	/// Derives the semi-latus rectum (l) of an orbit, given the semimajor axis (a) and eccentricity (e).
	/// @param a Semimajor axis (a).
	/// @param ec Eccentricity (e).
	/// @return Semi-latus rectum (l).
	template <class T>
	[[nodiscard]] constexpr T semilatus_rectum(T a, T ec) noexcept
	{
		return a * (T{1} - ec * ec);
	}
}

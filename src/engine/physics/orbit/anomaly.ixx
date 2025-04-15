// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.orbit.anomaly;
import engine.math.constants;
import engine.math.functions;
import engine.utility.sized_types;

using namespace engine::math;

/// Orbital anomaly functions.
export namespace engine::physics::orbit::anomaly
{
	/// Derives the eccentric anomaly given eccentricity and true anomaly.
	/// @param ec Eccentricity (e).
	/// @param ta True anomaly (nu).
	/// @return Eccentric anomaly (E).
	template <class T>
	[[nodiscard]] T true_to_eccentric(T ec, T ta)
	{
		if (ec == T{1})
		{
			// Parabolic orbit
			return tan(ta * T{0.5});
		}
		else if (ec > T{1})
		{
			// Hyperbolic orbit
			return acosh((ec + cos(ta)) / (T{1} + ec * cos(ta))) * ((ta < T{0}) ? T{-1} : T{1});
		}
		else
		{
			// Elliptic orbit
			return atan(sqrt(T{1} - sqr(ec)) * sin(ta), cos(ta) + ec);
		}
	}

	/// Derives the mean anomaly given eccentricity and eccentric anomaly.
	/// @param ec Eccentricity (e).
	/// @param ea Eccentric anomaly (E).
	/// @return Mean anomaly (M).
	template <class T>
	[[nodiscard]] T eccentric_to_mean(T ec, T ea)
	{
		if (ec == T{1})
		{
			// Parabolic orbit
			return cube(ea) / T{6} + ea * T{0.5};
		}
		else if (ec > T{1})
		{
			// Hyperbolic orbit
			return ec * sinh(ea) - ea;
		}
		else
		{
			// Elliptic orbit
			return ea - ec * sin(ea);
		}
	}

	/// Derives the mean anomaly given eccentricity and true anomaly.
	/// @param ec Eccentricity (e).
	/// @param ta True anomaly (nu).
	/// @return Mean anomaly (M).
	template <class T>
	[[nodiscard]] T true_to_mean(T ec, T ta)
	{
		return eccentric_to_mean(ec, true_to_eccentric(ec, ta));
	}

	/// Derives the true anomaly given eccentricity and eccentric anomaly.
	/// @param ec Eccentricity (e).
	/// @param ea Eccentric anomaly (E).
	/// @return True anomaly (nu).
	template <class T>
	[[nodiscard]] T eccentric_to_true(T ec, T ea)
	{
		if (ec == T{1})
		{
			// Parabolic orbit
			return atan(ea) * T{2};
		}
		else if (ec > T{1})
		{
			// Hyperbolic orbit
			return atan(sqrt((ec + T{1}) / (ec - T{1})) * tanh(ea * T{0.5})) * T{2};
		}
		else
		{
			// Elliptic orbit
			return atan(sqrt(T{1} - sqr(ec)) * sin(ea), cos(ea) - ec);
		}
	}

	/// Iteratively derives the eccentric anomaly given eccentricity and mean anomaly.
	/// @param ec Eccentricity (e).
	/// @param ma Mean anomaly (M).
	/// @param iterations Maximum number of iterations.
	/// @param tolerance Solution error tolerance.
	/// @return Eccentric anomaly (E).
	/// @see Murison, Marc. (2006). A Practical Method for Solving the Kepler Equation. 10.13140/2.1.5019.6808.
	template <class T>
	[[nodiscard]] T mean_to_eccentric(T ec, T ma, usize iterations, T tolerance)
	{
		// Wrap mean anomaly to `[-Pi, Pi]`
		ma = wrap_radians(ma);

		// Third-order approximation of eccentric anomaly starting value, E0
		const T t33 = cos(ma);
		const T t34 = sqr(ec);
		const T t35 = t34 * ec;
		T ea0 = ma + (T(-0.5) * t35 + ec + (t34 + T{1.5} * t33 * t35) * t33) * sin(ma);

		// Iteratively converge E0 and E1
		for (usize i = 0; i < iterations; ++i)
		{
			// Third-order approximation of eccentric anomaly, E1
			const T t1 = cos(ea0);
			const T t2 = T{-1} + ec * t1;
			const T t3 = sin(ea0);
			const T t4 = ec * t3;
			const T t5 = -ea0 + t4 + ma;
			const T t6 = t5 / (T{0.5} * t5 * t4 / t2 + t2);
			const T ea1 = ea0 - (t5 / ((T{0.5} * t3 - (T{1} / T{6}) * t1 * t6) * ec * t6 + t2));

			// Determine solution error
			const T error = abs(ea1 - ea0);

			// Set E0 to E1
			ea0 = ea1;

			// Break if solution is within error tolerance
			if (error < tolerance)
			{
				break;
			}
		}

		return ea0;
	}

	/// Iteratively derives the true anomaly given eccentricity and mean anomaly.
	/// @param ec Eccentricity (e).
	/// @param ma Mean anomaly (M).
	/// @param iterations Maximum number of iterations.
	/// @param tolerance Solution error tolerance.
	/// @return True anomaly (nu).
	template <class T>
	[[nodiscard]] T mean_to_true(T ec, T ma, usize iterations, T tolerance)
	{
		return eccentric_to_true(ec, mean_to_eccentric(ec, ma, iterations, tolerance));
	}
}

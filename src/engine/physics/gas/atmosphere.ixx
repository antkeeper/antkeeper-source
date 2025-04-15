// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.gas.atmosphere;
export import engine.math.vector;
import engine.physics.constants;
import engine.math.functions;
import engine.math.constants;
import engine.utility.sized_types;

/// Atmosphere-related functions.
export namespace engine::physics::gas::atmosphere
{
	/// Calculates a particle polarizability factor.
	/// @param ior Atmospheric index of refraction.
	/// @param density Molecular number density, in mol/m-3.
	/// @return Polarizability factor.
	/// @see Elek, O., & Kmoch, P. (2010). Real-time spectral scattering in large-scale natural participating media. Proceedings of the 26th Spring Conference on Computer Graphics - SCCG ’10. doi:10.1145/1925059.1925074
	/// @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
	template <class T>
	[[nodiscard]] T polarization(T ior, T density) noexcept
	{
		constexpr T k = T(2) * math::pi<T> *math::pi<T>;
		const T ior2m1 = ior * ior - T(1);
		const T num = k * ior2m1 * ior2m1;
		const T den = T(3) * density * density;
		return num / den;
	}

	/// Calculates a wavelength-dependent scattering coefficient.
	/// @param density Molecular number density of the particles, in mol/m-3.
	/// @param polarization Particle polarizability factor.
	/// @param wavelength Wavelength of light, in meters.
	/// @return Scattering coefficient.
	/// @see atmosphere::polarization
	/// @see Elek, O., & Kmoch, P. (2010). Real-time spectral scattering in large-scale natural participating media. Proceedings of the 26th Spring Conference on Computer Graphics - SCCG ’10. doi:10.1145/1925059.1925074
	/// @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
	template <class T>
	[[nodiscard]] T scattering(T density, T polarization, T wavelength) noexcept
	{
		const T wavelength2 = wavelength * wavelength;
		return math::four_pi<T> *(density / (wavelength2 * wavelength2)) * polarization;
	}

	/// Calculates a wavelength-independent scattering coefficient.
	/// @param density Molecular number density of the particles, in mol/m-3.
	/// @param polarization Particle polarizability factor.
	/// @return Scattering coefficient.
	/// @see atmosphere::polarization
	/// @see Elek, O., & Kmoch, P. (2010). Real-time spectral scattering in large-scale natural participating media. Proceedings of the 26th Spring Conference on Computer Graphics - SCCG ’10. doi:10.1145/1925059.1925074
	/// @see Elek, Oskar. (2009). Rendering Parametrizable Planetary Atmospheres with Multiple Scattering in Real-Time.
	template <class T>
	[[nodiscard]] T scattering(T density, T polarization) noexcept
	{
		return math::four_pi<T> *density * polarization;
	}

	/// Calculates an absorption coefficient.
	/// @param scattering Scattering coefficient.
	/// @param albedo Single-scattering albedo.
	/// @return Absorption coefficient.
	/// @see https://en.wikipedia.org/wiki/Single-scattering_albedo
	template <class T>
	[[nodiscard]] T absorption(T scattering, T albedo) noexcept
	{
		return scattering * (T(1) / albedo - T(1));
	}

	/// Calculates an extinction coefficient.
	/// @param scattering Scattering coefficient.
	/// @param albedo Single-scattering albedo.
	/// @return Extinction coefficient.
	/// @see https://en.wikipedia.org/wiki/Single-scattering_albedo
	template <class T>
	[[nodiscard]] T extinction(T scattering, T albedo) noexcept
	{
		return scattering / albedo;
	}

	/// Approximates the optical depth of exponentially-distributed atmospheric particles between two points using the trapezoidal rule.
	/// @param a Start point.
	/// @param b End point.
	/// @param r Radius of the planet.
	/// @param sh Scale height of the atmospheric particles.
	/// @param n Number of samples.
	/// @return Optical depth between @p a and @p b.
	template <class T>
	[[nodiscard]] T optical_depth_exp(const math::vec3<T>& a, const math::vec3<T>& b, T r, T sh, usize n)
	{
		sh = T(-1) / sh;

		const T h = math::length(b - a) / T(n);

		math::vec3<T> dy = (b - a) / T(n);
		math::vec3<T> y = a + dy;

		T f_x = math::exp((math::length(a) - r) * sh);
		T f_y = math::exp((math::length(y) - r) * sh);
		T sum = (f_x + f_y);

		for (usize i = 1; i < n; ++i)
		{
			f_x = f_y;
			y += dy;
			f_y = math::exp((math::length(y) - r) * sh);
			sum += (f_x + f_y);
		}

		return sum / T(2) * h;
	}

	/// Approximates the optical depth of triangularly-distributed atmospheric particles between two points using the trapezoidal rule.
	/// @param p0 Start point.
	/// @param p1 End point.
	/// @param r Radius of the planet.
	/// @param a Distribution lower limit.
	/// @param b Distribution upper limit.
	/// @param c Distribution upper mode.
	/// @param n Number of samples.
	/// @return Optical depth between @p a and @p b.
	template <class T>
	[[nodiscard]] T optical_depth_tri(const math::vec3<T>& p0, const math::vec3<T>& p1, T r, T a, T b, T c, usize n)
	{
		a = T(1) / (a - c);
		b = T(1) / (b - c);

		const T h = math::length(p1 - p0) / T(n);

		math::vec3<T> dy = (p1 - p0) / T(n);
		math::vec3<T> y = p0 + dy;

		T z = math::length(p0) - r;
		T f_x = math::max(T(0), math::max(T(0), c - z) * a - math::max(T(0), z - c) * b + T(1));

		z = math::length(y) - r;
		T f_y = math::max(T(0), math::max(T(0), c - z) * a - math::max(T(0), z - c) * b + T(1));
		T sum = (f_x + f_y);

		for (usize i = 1; i < n; ++i)
		{
			f_x = f_y;
			y += dy;

			z = math::length(y) - r;
			f_y = math::max(T(0), math::max(T(0), c - z) * a - math::max(T(0), z - c) * b + T(1));

			sum += (f_x + f_y);
		}

		return sum / T(2) * h;
	}

	/// Atmospheric density functions.
	namespace density
	{
		/// Calculates the density of exponentially-distributed atmospheric particles at a given elevation.
		/// @param d0 Density at sea level.
		/// @param z Height above sea level.
		/// @param sh Scale height of the particle type.
		/// @return Particle density at elevation @p z.
		/// @see https://en.wikipedia.org/wiki/Barometric_formula
		/// @see https://en.wikipedia.org/wiki/Scale_height
		template <class T>
		[[nodiscard]] T exponential(T d0, T z, T sh)
		{
			return d0 * math::exp(-z / sh);
		}

		/// Calculates the density of triangularly-distributed atmospheric particles at a given elevation.
		/// @param d0 Density at sea level.
		/// @param z Height above sea level.
		/// @param a Distribution lower limit.
		/// @param b Distribution upper limit.
		/// @param c Distribution mode.
		/// @return Particle density at elevation @p z.
		/// @see https://en.wikipedia.org/wiki/Triangular_distribution
		template <class T>
		[[nodiscard]] T triangular(T d0, T z, T a, T b, T c) noexcept
		{
			return d0 * math::max(T(0), math::max(T(0), c - z) / (a - c) - math::max(T(0), z - c) / (b - c) + T(1));
		}

	}
}

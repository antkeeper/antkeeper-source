// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.orbit.frame;
export import engine.math.se3;
export import engine.math.quaternion;
export import engine.math.vector;
import engine.math.constants;
import engine.math.functions;

using namespace engine::math;

/// Orbital reference frames.
export namespace engine::physics::orbit::frame
{
	/// Perifocal (PQW) frame.
	namespace pqw
	{
		/// Converts PQW coordinates from Cartesian to spherical.
		/// @param v PQW Cartesian coordinates.
		/// @return PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		template <class T>
		[[nodiscard]] vec3<T> spherical(const vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return vec3<T>
			{
				sqrt(xx_yy + v.z() * v.z()),
				atan(v.z(), sqrt(xx_yy)),
				atan(v.y(), v.x())
			};
		}
	
		/// Constructs spherical PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @param b Semiminor axis (b).
		/// @return PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		template <class T>
		[[nodiscard]] vec3<T> spherical(T ec, T a, T ea, T b)
		{
			const T x = a * (cos(ea) - ec);
			const T y = b * sin(ea);
			const T d = sqrt(x * x + y * y);
			const T ta = atan(y, x);
		
			return vec3<T>{d, T(0), ta};
		}
	
		/// Constructs spherical PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @return PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		template <class T>
		[[nodiscard]] vec3<T> spherical(T ec, T a, T ea)
		{
			const T b = a * sqrt(T(1) - ec * ec);
			return spherical<T>(ec, a, ea, b);
		}
	
		/// Converts PQW coordinates from spherical to Cartesian.
		/// @param v PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		/// @return PQW Cartesian coordinates.
		template <class T>
		[[nodiscard]] vec3<T> cartesian(const vec3<T>& v)
		{
			const T x = v[0] * cos(v[1]);
		
			return vec3<T>
			{
				x * cos(v[2]),
				x * sin(v[2]),
				v[0] * sin(v[1]),
			};
		}
	
		/// Constructs Cartesian PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @param b Semiminor axis (b).
		/// @return PQW Cartesian coordinates.
		template <class T>
		[[nodiscard]] vec3<T> cartesian(T ec, T a, T ea, T b)
		{
			return cartesian<T>(spherical<T>(ec, a, ea, b));
		}
	
		/// Constructs Cartesian PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @return PQW Cartesian coordinates.
		template <class T>
		[[nodiscard]] vec3<T> cartesian(T ec, T a, T ea)
		{
			return cartesian<T>(spherical<T>(ec, a, ea));
		}

		/// Constructs an SE(3) transformation from a PQW frame to a BCI frame.
		/// @param om Right ascension of the ascending node (OMEGA), in radians.
		/// @param in Orbital inclination (i), in radians.
		/// @param w Argument of periapsis (omega), in radians.
		/// @return PQW to BCI transformation.
		template <typename T>
		[[nodiscard]] se3<T> to_bci(T om, T in, T w)
		{
			/// @todo Use engine.math.euler_angles
			const quat<T> r = normalize
			(
				quat<T>::rotate_z(om) *
				quat<T>::rotate_x(in) *
				quat<T>::rotate_z(w)
			);
		
			return se3<T>{{T(0), T(0), T(0)}, r};
		}
	}

	/// Body-centered inertial (BCI) frame.
	namespace bci
	{
		/// Converts BCI coordinates from spherical to Cartesian.
		/// @param v BCI spherical coordinates, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		/// @return BCI Cartesian coordinates.
		template <class T>
		[[nodiscard]] vec3<T> cartesian(const vec3<T>& v)
		{
			const T x = v[0] * cos(v[1]);
		
			return vec3<T>
			{
				x * cos(v[2]),
				x * sin(v[2]),
				v[0] * sin(v[1]),
			};
		}
	
		/// Converts BCI coordinates from Cartesian to spherical.
		/// @param v BCI Cartesian coordinates.
		/// @return BCI spherical coordinates, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		template <class T>
		[[nodiscard]] vec3<T> spherical(const vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return vec3<T>
			{
				sqrt(xx_yy + v.z() * v.z()),
				atan(v.z(), sqrt(xx_yy)),
				atan(v.y(), v.x())
			};
		}
	
		/// Constructs an SE(3) transformation from a BCI frame to a BCBF frame.
		/// @param ra Right ascension of the north pole, in radians.
		/// @param dec Declination of the north pole, in radians.
		/// @param w Location of the prime meridian, as a rotation about the north pole, in radians.
		/// @return BCI to BCBF transformation.
		/// @see Archinal, B.A., A’Hearn, M.F., Bowell, E. et al. Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements: 2009. Celest Mech Dyn Astr 109, 101–135 (2011). https://doi.org/10.1007/s10569-010-9320-4
		template <typename T>
		[[nodiscard]] se3<T> to_bcbf(T ra, T dec, T w)
		{
			/// @todo Use engine.math.euler_angles
			const quat<T> r = normalize
			(
				mul(mul(quat<T>::rotate_z(-half_pi<T> - ra), quat<T>::rotate_x(dec - half_pi<T>)), quat<T>::rotate_z(-w))
			);
		
			return se3<T>{{T(0), T(0), T(0)}, r};
		}
	
		/// Constructs an SE(3) transformation from a BCI frame to a PQW frame.
		/// @param om Right ascension of the ascending node (OMEGA), in radians.
		/// @param in Orbital inclination (i), in radians.
		/// @param w Argument of periapsis (omega), in radians.
		/// @return BCI to PQW transformation.
		template <typename T>
		[[nodiscard]] se3<T> to_pqw(T om, T in, T w)
		{
			/// @todo Use engine.math.euler_angles
			const quat<T> r = normalize
			(
				quat<T>::rotate_z(-w) *
					quat<T>::rotate_x(-in) *
					quat<T>::rotate_z(-om)
			);
		
			return se3<T>{{T(0), T(0), T(0)}, r};
		}
	}

	/// Body-centered, body-fixed (BCBF) frame.
	namespace bcbf
	{
		/// Converts BCBF coordinates from spherical to Cartesian.
		/// @param v BCBF spherical coordinates, in the ISO order of radial distance, latitude (radians), and longitude (radians).
		/// @return BCBF Cartesian coordinates.
		template <class T>
		[[nodiscard]] vec3<T> cartesian(const vec3<T>& v)
		{
			const T x = v[0] * cos(v[1]);
		
			return vec3<T>
			{
				x * cos(v[2]),
				x * sin(v[2]),
				v[0] * sin(v[1]),
			};
		}
	
		/// Converts BCBF coordinates from Cartesian to spherical.
		/// @param v BCBF Cartesian coordinates.
		/// @return BCBF spherical coordinates, in the ISO order of radial distance, latitude (radians), and longitude (radians).
		template <class T>
		[[nodiscard]] vec3<T> spherical(const vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return vec3<T>
			{
				sqrt(xx_yy + v.z() * v.z()),
				atan(v.z(), sqrt(xx_yy)),
				atan(v.y(), v.x())
			};
		}
	
		/// Constructs an SE(3) transformation from a BCBF frame to a BCI frame.
		/// @param ra Right ascension of the north pole, in radians.
		/// @param dec Declination of the north pole, in radians.
		/// @param w Location of the prime meridian, as a rotation about the north pole, in radians.
		/// @return BCBF to BCI transformation.
		/// @see Archinal, B.A., A’Hearn, M.F., Bowell, E. et al. Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements: 2009. Celest Mech Dyn Astr 109, 101–135 (2011). https://doi.org/10.1007/s10569-010-9320-4
		template <class T>
		[[nodiscard]] se3<T> to_bci(T ra, T dec, T w)
		{
			/// @todo Use engine.math.euler_angles
			const quat<T> r = normalize
			(
				mul(mul(quat<T>::rotate_z(w), quat<T>::rotate_x(half_pi<T> - dec)), quat<T>::rotate_z(ra + half_pi<T>))
			);
		
			return se3<T>{{T(0), T(0), T(0)}, r};
		}
	
		/// Constructs an SE(3) transformation from a BCBF frame to an ENU frame.
		/// @param distance Radial distance of the observer from the center of the body.
		/// @param latitude Latitude of the observer, in radians.
		/// @param longitude Longitude of the observer, in radians.
		/// @return BCBF to ENU transformation.
		template <class T>
		[[nodiscard]] se3<T> to_enu(T distance, T latitude, T longitude)
		{
			/// @todo Use engine.math.euler_angles
			const vec3<T> t = {T(0), T(0), -distance};
			const quat<T> r = normalize
			(
				mul(quat<T>::rotate_x(-half_pi<T> + latitude), quat<T>::rotate_z(-longitude - half_pi<T>))
			);
		
			return se3<T>{t, r};
		}
	}

	/// East, North, Up (ENU) horizontal frame.
	namespace enu
	{
		/// Converts ENU coordinates from spherical to Cartesian.
		/// @param v ENU spherical coordinates, in the ISO order of radial distance, elevation (radians), and azimuth (radians).
		/// @return ENU Cartesian coordinates.
		template <class T>
		[[nodiscard]] vec3<T> cartesian(const vec3<T>& v)
		{
			const T x = v[0] * cos(v[1]);
			const T y = half_pi<T> - v[2];
		
			return vec3<T>
			{
				x * cos(y),
				x * sin(y),
				v[0] * sin(v[1]),
			};
		}
	
		/// Converts ENU coordinates from Cartesian to spherical.
		/// @param v ENU Cartesian coordinates.
		/// @return ENU spherical coordinates, in the ISO order of radial distance, elevation (radians), and azimuth (radians).
		template <class T>
		[[nodiscard]] vec3<T> spherical(const vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return vec3<T>
			{
				sqrt(xx_yy + v.z() * v.z()),
				atan(v.z(), sqrt(xx_yy)),
				half_pi<T> -atan(v.y(), v.x())
			};
		}
	
		/// Constructs an SE(3) transformation from an ENU frame to a BCBF frame.
		/// @param distance Radial distance of the observer from the center of the body.
		/// @param latitude Latitude of the observer, in radians.
		/// @param longitude Longitude of the observer, in radians.
		/// @return ENU to BCBF transformation.
		template <typename T>
		[[nodiscard]] se3<T> to_bcbf(T distance, T latitude, T longitude)
		{
			const vec3<T> t = {T(0), T(0), distance};

			/// @todo Use engine.math.euler_angles
			const quat<T> r = normalize
			(
				quat<T>::rotate_z(longitude + half_pi<T>) *
					quat<T>::rotate_x(half_pi<T> - latitude)
			);
		
			return se3<T>{r * t, r};
		}
	}
}

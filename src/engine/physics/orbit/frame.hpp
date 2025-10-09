// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/se3.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/constants.hpp>
#include <engine/math/functions.hpp>
#include <engine/math/euler-angles.hpp>

/// Orbital reference frames.
namespace engine::physics::orbit::frame
{
	/// Perifocal (PQW) frame.
	namespace pqw
	{
		/// Converts PQW coordinates from Cartesian to spherical.
		/// @param v PQW Cartesian coordinates.
		/// @return PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		template <class T>
		[[nodiscard]] math::vec3<T> spherical(const math::vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return math::vec3<T>
			{
				math::sqrt(xx_yy + v.z() * v.z()),
				math::atan(v.z(), math::sqrt(xx_yy)),
				math::atan(v.y(), v.x())
			};
		}
	
		/// Constructs spherical PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @param b Semiminor axis (b).
		/// @return PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		template <class T>
		[[nodiscard]] math::vec3<T> spherical(T ec, T a, T ea, T b)
		{
			const T x = a * (math::cos(ea) - ec);
			const T y = b * math::sin(ea);
			const T d = math::sqrt(x * x + y * y);
			const T ta = math::atan(y, x);
		
			return math::vec3<T>{d, T{0}, ta};
		}
	
		/// Constructs spherical PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @return PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		template <class T>
		[[nodiscard]] math::vec3<T> spherical(T ec, T a, T ea)
		{
			const T b = a * math::sqrt(T{1} - ec * ec);
			return spherical<T>(ec, a, ea, b);
		}
	
		/// Converts PQW coordinates from spherical to Cartesian.
		/// @param v PQW spherical coordinates, in the ISO order of radial distance, inclination (radians), and true anomaly (radians).
		/// @return PQW Cartesian coordinates.
		template <class T>
		[[nodiscard]] math::vec3<T> cartesian(const math::vec3<T>& v)
		{
			const T x = v[0] * math::cos(v[1]);
		
			return math::vec3<T>
			{
				x * math::cos(v[2]),
				x * math::sin(v[2]),
				v[0] * math::sin(v[1]),
			};
		}
	
		/// Constructs Cartesian PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @param b Semiminor axis (b).
		/// @return PQW Cartesian coordinates.
		template <class T>
		[[nodiscard]] math::vec3<T> cartesian(T ec, T a, T ea, T b)
		{
			return cartesian<T>(spherical<T>(ec, a, ea, b));
		}
	
		/// Constructs Cartesian PQW coordinates from Keplerian orbital elements.
		/// @param ec Eccentricity (e).
		/// @param a Semimajor axis (a).
		/// @param ea Eccentric anomaly (E), in radians.
		/// @return PQW Cartesian coordinates.
		template <class T>
		[[nodiscard]] math::vec3<T> cartesian(T ec, T a, T ea)
		{
			return cartesian<T>(spherical<T>(ec, a, ea));
		}

		/// Constructs an SE(3) transformation from a PQW frame to a BCI frame.
		/// @param om Right ascension of the ascending node (OMEGA), in radians.
		/// @param in Orbital inclination (i), in radians.
		/// @param w Argument of periapsis (omega), in radians.
		/// @return PQW to BCI transformation.
		template <typename T>
		[[nodiscard]] math::se3<T> to_bci(T om, T in, T w)
		{
			return math::se3<T>{{}, math::euler_zxz_to_quat<T>({w, in, om})};
		}
	}

	/// Body-centered inertial (BCI) frame.
	namespace bci
	{
		/// Converts BCI coordinates from spherical to Cartesian.
		/// @param v BCI spherical coordinates, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		/// @return BCI Cartesian coordinates.
		template <class T>
		[[nodiscard]] math::vec3<T> cartesian(const math::vec3<T>& v)
		{
			const T x = v[0] * math::cos(v[1]);
		
			return math::vec3<T>
			{
				x * math::cos(v[2]),
				x * math::sin(v[2]),
				v[0] * math::sin(v[1]),
			};
		}
	
		/// Converts BCI coordinates from Cartesian to spherical.
		/// @param v BCI Cartesian coordinates.
		/// @return BCI spherical coordinates, in the ISO order of radial distance, declination (radians), and right ascension (radians).
		template <class T>
		[[nodiscard]] math::vec3<T> spherical(const math::vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return math::vec3<T>
			{
				math::sqrt(xx_yy + v.z() * v.z()),
				math::atan(v.z(), math::sqrt(xx_yy)),
				math::atan(v.y(), v.x())
			};
		}
	
		/// Constructs an SE(3) transformation from a BCI frame to a BCBF frame.
		/// @param ra Right ascension of the north pole, in radians.
		/// @param dec Declination of the north pole, in radians.
		/// @param w Location of the prime meridian, as a rotation about the north pole, in radians.
		/// @return BCI to BCBF transformation.
		/// @see Archinal, B.A., A’Hearn, M.F., Bowell, E. et al. Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements: 2009. Celest Mech Dyn Astr 109, 101–135 (2011). https://doi.org/10.1007/s10569-010-9320-4
		template <typename T>
		[[nodiscard]] math::se3<T> to_bcbf(T ra, T dec, T w)
		{
			return math::se3<T>{{}, math::euler_zxz_to_quat<T>({-w, dec - math::half_pi<T>, -math::half_pi<T> - ra})};
		}
	
		/// Constructs an SE(3) transformation from a BCI frame to a PQW frame.
		/// @param om Right ascension of the ascending node (OMEGA), in radians.
		/// @param in Orbital inclination (i), in radians.
		/// @param w Argument of periapsis (omega), in radians.
		/// @return BCI to PQW transformation.
		template <typename T>
		[[nodiscard]] math::se3<T> to_pqw(T om, T in, T w)
		{
			return math::se3<T>{{}, math::euler_zxz_to_quat<T>({-om, -in, -w})};
		}
	}

	/// Body-centered, body-fixed (BCBF) frame.
	namespace bcbf
	{
		/// Converts BCBF coordinates from spherical to Cartesian.
		/// @param v BCBF spherical coordinates, in the ISO order of radial distance, latitude (radians), and longitude (radians).
		/// @return BCBF Cartesian coordinates.
		template <class T>
		[[nodiscard]] math::vec3<T> cartesian(const math::vec3<T>& v)
		{
			const T x = v[0] * math::cos(v[1]);
		
			return math::vec3<T>
			{
				x * math::cos(v[2]),
				x * math::sin(v[2]),
				v[0] * math::sin(v[1]),
			};
		}
	
		/// Converts BCBF coordinates from Cartesian to spherical.
		/// @param v BCBF Cartesian coordinates.
		/// @return BCBF spherical coordinates, in the ISO order of radial distance, latitude (radians), and longitude (radians).
		template <class T>
		[[nodiscard]] math::vec3<T> spherical(const math::vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return math::vec3<T>
			{
				math::sqrt(xx_yy + v.z() * v.z()),
				math::atan(v.z(), math::sqrt(xx_yy)),
				math::atan(v.y(), v.x())
			};
		}
	
		/// Constructs an SE(3) transformation from a BCBF frame to a BCI frame.
		/// @param ra Right ascension of the north pole, in radians.
		/// @param dec Declination of the north pole, in radians.
		/// @param w Location of the prime meridian, as a rotation about the north pole, in radians.
		/// @return BCBF to BCI transformation.
		/// @see Archinal, B.A., A’Hearn, M.F., Bowell, E. et al. Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements: 2009. Celest Mech Dyn Astr 109, 101–135 (2011). https://doi.org/10.1007/s10569-010-9320-4
		template <class T>
		[[nodiscard]] math::se3<T> to_bci(T ra, T dec, T w)
		{
			return math::se3<T>{{}, math::euler_zxz_to_quat<T>({ra + math::half_pi<T>, math::half_pi<T> - dec, w})};
		}
	
		/// Constructs an SE(3) transformation from a BCBF frame to an ENU frame.
		/// @param distance Radial distance of the observer from the center of the body.
		/// @param latitude Latitude of the observer, in radians.
		/// @param longitude Longitude of the observer, in radians.
		/// @return BCBF to ENU transformation.
		template <class T>
		[[nodiscard]] math::se3<T> to_enu(T distance, T latitude, T longitude)
		{
			const math::vec3<T> t = {T{0}, T{0}, -distance};
			const math::quat<T> r = math::euler_zxz_to_quat<T>({-longitude - math::half_pi<T>, -math::half_pi<T> + latitude, T{0}});
			return math::se3<T>{t, r};
		}
	}

	/// East, North, Up (ENU) horizontal frame.
	namespace enu
	{
		/// Converts ENU coordinates from spherical to Cartesian.
		/// @param v ENU spherical coordinates, in the ISO order of radial distance, elevation (radians), and azimuth (radians).
		/// @return ENU Cartesian coordinates.
		template <class T>
		[[nodiscard]] math::vec3<T> cartesian(const math::vec3<T>& v)
		{
			const T x = v[0] * math::cos(v[1]);
			const T y = math::half_pi<T> - v[2];
		
			return math::vec3<T>
			{
				x * math::cos(y),
				x * math::sin(y),
				v[0] * math::sin(v[1]),
			};
		}
	
		/// Converts ENU coordinates from Cartesian to spherical.
		/// @param v ENU Cartesian coordinates.
		/// @return ENU spherical coordinates, in the ISO order of radial distance, elevation (radians), and azimuth (radians).
		template <class T>
		[[nodiscard]] math::vec3<T> spherical(const math::vec3<T>& v)
		{
			const T xx_yy = v.x() * v.x() + v.y() * v.y();
		
			return math::vec3<T>
			{
				math::sqrt(xx_yy + v.z() * v.z()),
				math::atan(v.z(), math::sqrt(xx_yy)),
				math::half_pi<T> -math::atan(v.y(), v.x())
			};
		}
	
		/// Constructs an SE(3) transformation from an ENU frame to a BCBF frame.
		/// @param distance Radial distance of the observer from the center of the body.
		/// @param latitude Latitude of the observer, in radians.
		/// @param longitude Longitude of the observer, in radians.
		/// @return ENU to BCBF transformation.
		template <typename T>
		[[nodiscard]] math::se3<T> to_bcbf(T distance, T latitude, T longitude)
		{
			const math::vec3<T> t = {T{0}, T{0}, distance};
			const math::quat<T> r = math::euler_zxz_to_quat<T>({T{0}, math::half_pi<T> - latitude, longitude + math::half_pi<T>});
			return math::se3<T>{r * t, r};
		}
	}
}

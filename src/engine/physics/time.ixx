// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.time;
import engine.math.constants;

/// Time-related functions.
export namespace engine::physics::time
{
	/// Number of seconds per day, in seconds.
	template <class T>
	inline constexpr T seconds_per_day = T{86400};

	/// Julian day (JD).
	namespace jd
	{
		/// Number of Julian days per Julian century.
		template <class T>
		inline constexpr T days_per_century = T{36525};

		/// Number of Julian centuries per Julian day.
		template <class T>
		inline constexpr T centuries_per_day = T{1} / T{36525};

		/// Converts JD to UT1.
		/// @param t JD time.
		/// @return UT1 time.
		template <class T>
		[[nodiscard]] inline constexpr T to_ut1(T t) noexcept
		{
			return t - T{2451545};
		}
	}

	/// Gregorian calendar time.
	namespace gregorian
	{
		/// Calculates the JD time from a Gregorian date and time. Valid for all dates after November 23, −4713.
		/// @param year Astronomical year numbering. 1 BC is `0`, 2 BC is `-1`.
		/// @param month Month number on `[1, 12]`.
		/// @param day Day number on `[1, 31]`.
		/// @param hour Hour number on `[0, 23]`.
		/// @param minute Minute number on `[0, 59]`.
		/// @param second Fractional second on `[0.0, 60.0)`.
		/// @param utc UTC offset.
		/// @return JD time.
		/// @see L. E. Doggett, Ch. 12, "Calendars", p. 606, in Seidelmann 1992
		template <class T>
		[[nodiscard]] constexpr T to_jd(int year, int month, int day, int hour, int minute, T second, T utc) noexcept
		{
			const T jdn = static_cast<T>((1461 * (year + 4800 + (month - 14) / 12)) / 4 + (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12 - (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4 + day - 32075);
			return jdn + static_cast<T>(hour - 12) / T{24} + static_cast<T>(minute) / T{1440} + static_cast<T>(second) / T{86400} - utc / T{24};
		}

		/// Calculates the UT1 time from a Gregorian date and time. Valid for all dates after November 23, −4713.
		/// @param year Astronomical year numbering. 1 BC is `0`, 2 BC is `-1`.
		/// @param month Month number on `[1, 12]`.
		/// @param day Day number on `[1, 31]`.
		/// @param hour Hour number on `[0, 23]`.
		/// @param minute Minute number on `[0, 59]`.
		/// @param second Fractional second on `[0.0, 60.0)`.
		/// @param utc UTC offset.
		/// @return UT1 time.
		template <class T>
		[[nodiscard]] constexpr T to_ut1(int year, int month, int day, int hour, int minute, T second, T utc) noexcept
		{
			return jd::to_ut1<T>(to_jd<T>(year, month, day, hour, minute, second, utc));
		}
	}

	/// UT1 universal time.
	namespace ut1
	{
		/// Converts UT1 to JD.
		/// @param t UT1 time.
		/// @return JD time.
		template <class T>
		[[nodiscard]] inline constexpr T to_jd(T t) noexcept
		{
			return t + T{2451545};
		}

		/// Calculates the Earth Rotation Angle (ERA) at a given UT1 date.
		/// @param t J2000 UT1 date.
		/// @return ERA at the given date, in radians.
		template <class T>
		[[nodiscard]] inline constexpr T era(T t) noexcept
		{
			return math::two_pi<T> * (T{0.7790572732640} + T{1.00273781191135448} *t);
		}
	}

	/// Coordinated Universal Time (UTC).
	namespace utc
	{
		/// Calculates the UTC offset at a given longitude
		/// @param longitude Longitude, in radians.
		/// @return UTC offset.
		template <class T>
		[[nodiscard]] inline constexpr T offset(T longitude) noexcept
		{
			return longitude / (math::two_pi<T> / T{24});
		}
	}
}

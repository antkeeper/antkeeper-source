// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/constants.hpp>
#include <engine/utility/sized-types.hpp>
#include <tuple>
#include <cmath>
#include <concepts>

/// Common mathematical functions.
namespace engine::math::inline functions
{
	/// @name Basic operations
	/// @{

	/// Returns the minimum of two values.
	/// @param a First value.
	/// @param b Second value.
	/// @return `a < b ? a : b`.
	template <class T>
	[[nodiscard]] inline constexpr const T& min(const T& a, const T& b) noexcept
	{
		return a < b ? a : b;
	}

	/// Returns the maximum of two values.
	/// @param a First value.
	/// @param b Second value.
	/// @return `a > b ? a : b`.
	template <class T>
	[[nodiscard]] inline constexpr const T& max(const T& a, const T& b) noexcept
	{
		return a > b ? a : b;
	}

	/// Constrains a value to lie within a range.
	/// @param x Value to constrain.
	/// @param min_val Lower bounds.
	/// @param max_val Upper bounds.
	/// @return @p x constrained to [`min_val`, `max_val`].
	template <class T>
	[[nodiscard]] inline constexpr const T& clamp(const T& x, const T& min_val, const T& max_val) noexcept
	{
		return min(max(x, min_val), max_val);
	}

	/// Returns the absolute value of a number.
	/// @param x Floating-point value.
	/// @return Absolute value of @p x.
	template <class T>
		requires (std::is_floating_point_v<T> || (std::is_integral_v<T> && std::is_signed_v<T>))
	[[nodiscard]] inline constexpr T abs(T x)
	{
		return std::abs(x);
	}

	/// Returns the nearest integer not less than the given value.
	/// @param x Floating-point value.
	/// @return Nearest integer not less than @p x.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T ceil(T x)
	{
		return std::ceil(x);
	}

	/// Returns the nearest integer not greater than the given value.
	/// @param x Floating-point value.
	/// @return Nearest integer not greater than @p x.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T floor(T x)
	{
		return std::floor(x);
	}

	/// Rounds to the nearest integer value, rounding halfway cases away from zero.
	/// @param x Floating-point value.
	/// @return Nearest integer value.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T round(T x)
	{
		return std::round(x);
	}

	/// Rounds to the nearest integer value, rounding halfway cases to nearest even integer.
	/// @param x Floating-point value.
	/// @return Nearest integer value.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T roundeven(T x)
	{
		T i;
		T f = std::modf(x, &i);

		if (std::abs(f) != T{0.5})
		{
			return std::round(x);
		}

		return (static_cast<isize>(i) % 2 == 0) ? i : i + (x > T{0} ? T{1} : T{-1});
	}

	/// Calculates the reciprocal of a value.
	/// @param x Floating-point value.
	/// @return Reciprocal of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T rcp(T x) noexcept
	{
		return T{1} / x;
	}

	/// Performs a multiply-add operation.
	/// @param x Input value.
	/// @param y Value to multiply.
	/// @param z Value to add.
	/// @return Multiply-add result.
	template <class T>
		requires (std::is_arithmetic_v<T>)
	[[nodiscard]] inline constexpr T fma(T x, T y, T z) noexcept
	{
		return std::fma(x, y, z);
	}

	/// @}

	/// @name Angular conversion
	/// @{

	/// Converts an angle from radians to degrees.
	/// @param radians Angle in radians.
	/// @return Angle in degrees.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T degrees(T radians) noexcept
	{
		return radians * rad2deg<T>;
	}

	/// Converts an angle from degrees to radians.
	/// @param degrees Angle in degrees.
	/// @return Angle in radians.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T radians(T degrees) noexcept
	{
		return degrees * deg2rad<T>;
	}

	/// Wraps an angle to [-180, 180].
	/// @param angle Angle, in degrees.
	/// @return Angle on [-180, 180].
	template <class T>
	[[nodiscard]] inline constexpr T wrap_degrees(T angle)
	{
		return std::remainder(angle, T{360});
	}

	/// Wraps an angle to [-Pi, Pi].
	/// @param angle Angle, in radians.
	/// @return Angle on [-Pi, Pi].
	template <class T>
	[[nodiscard]] inline constexpr T wrap_radians(T angle)
	{
		return std::remainder(angle, two_pi<T>);
	}

	/// @}

	/// @name Exponential functions
	/// @{

	/// Raises *e* to the given power.
	/// @param x Exponent value.
	/// @return *e* raised to the power of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T exp(T x)
	{
		return std::exp(x);
	}

	/// Raises *e* to the given power and subtracts `1`.
	/// @param x Exponent value.
	/// @return *e* raised to the power of @p x, minus `1`.
	template <std::floating_point T>
	[[nodiscard]] inline T expm1(T x)
	{
		return std::expm1(x);
	}

	/// Raises 2 to the given power.
	/// @param x Exponent value.
	/// @return 2 raised to the power of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T exp2(T x)
	{
		return std::exp2(x);
	}

	/// Raises 10 to the given power.
	/// @param x Exponent value.
	/// @return 10 raised to the power of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T exp10(T x)
	{
		return std::pow(T{10}, x);
	}

	/// Returns the natural (base *e*) logarithm of a given value.
	/// @param x Floating-point value.
	/// @return Natural (base *e*) logarithm of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T log(T x)
	{
		return std::log(x);
	}

	/// Returns the base 2 logarithm of a given value.
	/// @param x Floating-point value.
	/// @return Base 2 logarithm of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T log2(T x)
	{
		return std::log2(x);
	}

	/// Returns the base 10 logarithm of a given value.
	/// @param x Floating-point value.
	/// @return Base 10 logarithm of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T log10(T x)
	{
		return std::log10(x);
	}

	/// @}

	/// @name Power functions
	/// @{

	/// Raises a number to the given power.
	/// @param base Base value.
	/// @param exp Exponent value.
	/// @return @p base raised to the power of @p exp.
	template <std::floating_point T>
	[[nodiscard]] inline T pow(T base, T exp)
	{
		return std::pow(base, exp);
	}

	/// Raises a number to the power of 2.
	/// @param base Base value.
	/// @return @p base raised to the power of 2.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T sqr(T base) noexcept
	{
		return base * base;
	}

	/// Raises a number to the power of 3.
	/// @param base Base value.
	/// @return @p base raised to the power of 3.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T cube(T base) noexcept
	{
		return base * base * base;
	}

	/// Returns the square root of a value.
	/// @param x Floating-point value.
	/// @return Square root of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T sqrt(T x)
	{
		return std::sqrt(x);
	}

	/// Returns the inverse of the square root of a value.
	/// @param x Floating-point value.
	/// @return Inverse of the square root of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T rcp_sqrt(T x)
	{
		return rcp(sqrt(x));
	}

	/// Returns the cube root of a value.
	/// @param x Floating-point value.
	/// @return Cube root of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline T cbrt(T x)
	{
		return std::cbrt(x);
	}

	/// @}

	/// @name Trigonometric functions
	/// @{

	/// Returns the sine of an angle.
	/// @param angle Angle, in radians.
	/// @return Sine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T sin(T angle)
	{
		return std::sin(angle);
	}

	/// Returns the cosine of an angle.
	/// @param angle Angle, in radians.
	/// @return Cosine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T cos(T angle)
	{
		return std::cos(angle);
	}

	/// Returns the tangent of an angle.
	/// @param angle Angle, in radians.
	/// @return Tangent of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T tan(T angle)
	{
		return std::tan(angle);
	}

	/// Returns the arcsine of an angle.
	/// @param angle Angle, in radians.
	/// @return Arcsine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T asin(T angle)
	{
		return std::asin(angle);
	}

	/// Returns the arccosine of an angle.
	/// @param angle Angle, in radians.
	/// @return Arccosine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T acos(T angle)
	{
		return std::acos(angle);
	}

	/// Returns the arctangent of @p y / @p x.
	/// @param y Numerator.
	/// @param x Denominator.
	/// @return Arctangent of @p y / @p x, in radians.
	template <std::floating_point T>
	[[nodiscard]] inline T atan(T y, T x)
	{
		return std::atan2(y, x);
	}

	/// Returns the arctangent of y / x.
	/// @param y_over_x y / x.
	/// @return Arctangent of y / x, in radians.
	template <std::floating_point T>
	[[nodiscard]] inline T atan(T y_over_x)
	{
		return std::atan(y_over_x);
	}

	/// @}

	/// @name Hyperbolic functions
	/// @{

	/// Returns the hyperbolic sine of an angle.
	/// @param angle Angle, in radians.
	/// @return Hyperbolic sine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T sinh(T angle)
	{
		return std::sinh(angle);
	}

	/// Returns the hyperbolic cosine of an angle.
	/// @param angle Angle, in radians.
	/// @return Hyperbolic cosine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T cosh(T angle)
	{
		return std::cosh(angle);
	}

	/// Returns the hyperbolic tangent of an angle.
	/// @param angle Angle, in radians.
	/// @return Hyperbolic tangent of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T tanh(T angle)
	{
		return std::tanh(angle);
	}

	/// Returns the inverse hyperbolic sine of an angle.
	/// @param angle Angle, in radians.
	/// @return Inverse hyperbolic sine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T asinh(T angle)
	{
		return std::asinh(angle);
	}

	/// Returns the inverse hyperbolic cosine of an angle.
	/// @param angle Angle, in radians.
	/// @return Inverse hyperbolic cosine of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T acosh(T angle)
	{
		return std::acosh(angle);
	}

	/// Returns the inverse hyperbolic tangent of an angle.
	/// @param angle Angle, in radians.
	/// @return Inverse hyperbolic tangent of @p angle.
	template <std::floating_point T>
	[[nodiscard]] inline T atanh(T angle)
	{
		return std::atanh(angle);
	}

	/// @}

	/// @name Sign functions
	/// @{
	
	/// Extracts the sign of a floating-point value with a specified magnitude.
	/// @param magnitude Magnitude of the result.
	/// @param x Value from which to extract the sign.
	/// @return Sign of @p x with the given magnitude.
	template <class T>
		requires (std::is_arithmetic_v<T>)
	[[nodiscard]] inline constexpr T copysign(T magnitude, T x)
	{
		return std::copysign(magnitude, x);
	}

	/// Extracts the sign of a floating-point value.
	/// @param x Floating-point value.
	/// @return Sign of @p x with a magnitude of 1.
	template <class T>
		requires (std::is_arithmetic_v<T>)
	[[nodiscard]] inline constexpr T sign(T x)
	{
		return copysign(T{1}, x);
	}

	/// Determines whether a value is negative.
	/// @param x Signed value.
	/// @return `true` if the @p x is negative, `false` otherwise.
	template <class T>
		requires (std::is_floating_point_v<T> || (std::is_integral_v<T> && std::is_signed_v<T>))
	[[nodiscard]] inline constexpr bool signbit(T x)
	{
		return std::signbit(x);
	}

	/// @}

	/// @name Floating-point manipulation functions
	/// @{

	/// Returns the fractional part of a floating-point value for positive values. Implemented as `x - floor(x)`.
	/// @param x Floating-point value.
	/// @return Fractional part of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T fract(T x)
	{
		return x - floor(x);
	}

	/// Returns @p x with the fractional part removed.
	/// @param x Floating-point value.
	/// @return Integer part of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T trunc(T x)
	{
		return std::trunc(x);
	}

	/// Returns the value of @p x modulo @p y.
	/// @param x First value.
	/// @param y Second value.
	/// @return @p x modulo @p y.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T mod(T x, T y)
	{
		return x - y * trunc(x / y);
	}

	/// Splits a value into its fractional and integer components.
	/// @param x Value to split.
	/// @return Tuple containing the fractional part of @p x, followed by the integer part of @p x.
	template <std::floating_point T>
	[[nodiscard]] inline std::tuple<T, T> modf(T x)
	{
		std::tuple<T, T> result;
		std::get<0>(result) = std::modf(x, &std::get<1>(result));
		return result;
	}

	/// @}

	/// @name Interpolation functions
	/// @{

	/// Linearly interpolates between two values.
	/// @param a First value.
	/// @param b Second value.
	/// @param t Interpolation factor.
	/// @return Linearly-interpolated value.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr T lerp(T a, T b, T t) noexcept
	{
		// Imprecise:
		// return (b - a) * t + a;

		// Precise:
		return a * (T{1} - t) + b * t;
	}

	/// Linearly interpolates between two angles, @p x and @p y.
	/// @param x Start angle, in radians.
	/// @param y End angle, in radians.
	/// @param a Interpolation ratio.
	/// @return Interpolated angle, in radians.
	template <std::floating_point T>
	[[nodiscard]] constexpr T lerp_angle(T x, T y, T a)
	{
		return wrap_radians(x + wrap_radians(y - x) * a);
	}

	/// Logarithmically interpolates between two values.
	/// @param a First value.
	/// @param b Second value.
	/// @param t Interpolation factor.
	/// @warning Undefined behavior when @p a is zero.
	template <std::floating_point T>
	[[nodiscard]] T log_lerp(T a, T b, T t)
	{
		//return exp(lerp(log(x), log(y), a));
		return a * pow(b / a, t);
	}

	/// @}

	/// @name Classification and comparison 
	/// @{

	/// Returns `true` if a value is NaN, `false` otherwise.
	/// @param x Value to evaluate.
	/// @return `true` if @p x is NaN, `false` otherwise.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr bool isnan(T x) noexcept
	{
		return std::isnan(x);
	}

	/// Returns `true` if a value is infinite, `false` otherwise.
	/// @param x Value to evaluate.
	/// @return `true` if @p x is infinite, `false` otherwise.
	template <std::floating_point T>
	[[nodiscard]] inline constexpr bool isinf(T x) noexcept
	{
		return std::isinf(x);
	}

	/// @}

	/// Remaps a number from one range to another.
	/// @param x Value to remap.
	/// @param from_min Start of the first range.
	/// @param from_max End of the first range.
	/// @param to_min Start of the second range.
	/// @param to_max End of the second range.
	/// @return Unclamped remapped value.
	template <std::floating_point T>
	[[nodiscard]] constexpr T map_range(T x, T from_min, T from_max, T to_min, T to_max) noexcept
	{
		return to_min + (x - from_min) * (to_max - to_min) / (from_max - from_min);
	}
}

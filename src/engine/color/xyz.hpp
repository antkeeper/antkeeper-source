// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/functions.hpp>

namespace engine::color
{
	/// @name CIE XYZ color space
	/// @{

	/// Returns the luminance of a CIE XYZ color.
	/// @tparam T Scalar type.
	/// @param x CIE XYZ color.
	/// @return return Luminance of @p x.
	template <class T>
	[[nodiscard]] inline constexpr T xyz_to_luminance(const math::vec3<T>& x) noexcept
	{
		return x[1];
	}

	/// Transforms a CIE XYZ color into the CIE xyY color space.
	/// @tparam T Scalar type.
	/// @param x CIE XYZ color.
	/// @return CIE xyY color.
	template <class T>
	[[nodiscard]] constexpr math::vec3<T> xyz_to_xyy(const math::vec3<T>& x) noexcept
	{
		const T sum = x[0] + x[1] + x[2];
		return math::vec3<T>{x[0] / sum, x[1] / sum, x[1]};
	}

	/// CIE 1931 standard observer color matching function for the X tristimulus value.
	/// @tparam T Scalar type.
	/// @param lambda Wavelength of light, in nanometers.
	/// @return Matching X tristimulus value.
	/// @see xyz_match()
	template <class T>
	[[nodiscard]] T xyz_match_x(T lambda)
	{
		const T t0 = (lambda - T{442.0}) * ((lambda < T{442.0}) ? T{0.0624} : T{0.0374});
		const T t1 = (lambda - T{599.8}) * ((lambda < T{599.8}) ? T{0.0264} : T{0.0323});
		const T t2 = (lambda - T{501.1}) * ((lambda < T{501.1}) ? T{0.0490} : T{0.0382});
	
		const T x0 = T{ 0.362} * exp(T{-0.5} * t0 * t0);
		const T x1 = T{ 1.056} * exp(T{-0.5} * t1 * t1);
		const T x2 = T{-0.065} * exp(T{-0.5} * t2 * t2);
	
		return x0 + x1 + x2;
	}

	/// CIE 1931 standard observer color matching function for the Y tristimulus value.
	/// @tparam T Scalar type.
	/// @param lambda Wavelength of light, in nanometers.
	/// @return Matching Y tristimulus value.
	/// @see xyz_match()
	template <class T>
	[[nodiscard]] T xyz_match_y(T lambda)
	{
		const T t0 = (lambda - T{568.8}) * ((lambda < T{568.8}) ? T{0.0213} : T{0.0247});
		const T t1 = (lambda - T{530.9}) * ((lambda < T{530.9}) ? T{0.0613} : T{0.0322});
	
		const T y0 = T{0.821} * exp(T{-0.5} * t0 * t0);
		const T y1 = T{0.286} * exp(T{-0.5} * t1 * t1);
	
		return y0 + y1;
	}

	/// CIE 1931 standard observer color matching function for the Z tristimulus value.
	/// @tparam T Scalar type.
	/// @param lambda Wavelength of light, in nanometers.
	/// @return Matching Z tristimulus value.
	/// @see xyz_match()
	template <class T>
	[[nodiscard]] T xyz_match_z(T lambda)
	{
		const T t0 = (lambda - T{437.0}) * ((lambda < T{437.0}) ? T{0.0845} : T{0.0278});
		const T t1 = (lambda - T{459.0}) * ((lambda < T{459.0}) ? T{0.0385} : T{0.0725});
	
		const T z0 = T{1.217} * exp(T{-0.5} * t0 * t0);
		const T z1 = T{0.681} * exp(T{-0.5} * t1 * t1);
	
		return z0 + z1;
	}

	/// Fitted piecewise gaussian approximation to the CIE 1931 standard observer color matching function.
	/// @tparam T Scalar type.
	/// @param lambda Wavelength of light, in nanometers.
	/// @return Matching CIE XYZ color.
	/// @see xyz_match_x()
	/// @see xyz_match_y()
	/// @see xyz_match_z()
	/// @see Wyman, C., Sloan, P.J., & Shirley, P. (2013). Simple Analytic Approximations to the CIE XYZ Color Matching Functions.
	template <class T>
	[[nodiscard]] math::vec3<T> xyz_match(T lambda)
	{
		return math::vec3<T>
		{
			xyz_match_x<T>(lambda),
			xyz_match_y<T>(lambda),
			xyz_match_z<T>(lambda)
		};
	}

	/// @}
}

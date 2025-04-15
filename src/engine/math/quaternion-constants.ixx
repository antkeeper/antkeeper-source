// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.math.quaternion:constants;
import :type;
import engine.math.constants;

export namespace engine::math::inline constants
{
	/// @name Quaternion constants
	/// @{

	/// Quaternion of zeros.
	template <class T>
	inline constexpr quaternion<T> zero<quaternion<T>> = {};

	/// Quaternion of ones.
	template <class T>
	inline constexpr quaternion<T> one<quaternion<T>> = {one<T>, {one<T>, one<T>, one<T>}};

	/// Quaternion of infinities.
	template <class T>
	inline constexpr quaternion<T> inf<quaternion<T>> = {inf<T>, {inf<T>, inf<T>, inf<T>}};

	/// Rotational identity quaternion.
	template <class T>
	inline constexpr quaternion<T> identity<quaternion<T>> = {one<T>, {}};

	/// @}
}

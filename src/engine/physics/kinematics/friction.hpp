// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once


namespace engine::physics
{
	/// Specifies how coefficients of friction should be calculated.
	/// @details A coefficient of friction is calculated from two collider material friction values (`a` and `b`).
	enum class friction_combine_mode
	{
		/// Coefficient of friction is calculated as `(a + b) / 2`.
		average,

		/// Coefficient of friction is calculated as `min(a, b)`.
		/// @note Takes priority over friction_combine_mode::average
		minimum,

		/// Coefficient of friction is calculated as `a * b`.
		/// @note Takes priority over friction_combine_mode::average and friction_combine_mode::minimum.
		multiply,

		/// Coefficient of friction is calculated as `max(a, b)`.
		/// @note Takes priority over friction_combine_mode::average, friction_combine_mode::minimum, and friction_combine_mode::multiply.
		maximum
	};

	/// Combines two friction values into a coefficient of friction.
	/// @param a First friction value.
	/// @param b Second friction value.
	/// @param mode Friction combine mode.
	/// @return Coefficient of friction of @p a and @p b.
	[[nodiscard]] float combine_friction(float a, float b, friction_combine_mode mode) noexcept;
}

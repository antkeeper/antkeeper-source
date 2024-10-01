// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_RESTITUTION_HPP
#define ANTKEEPER_PHYSICS_RESTITUTION_HPP

#include <cstdint>

namespace physics {

/// Specifies how coefficients of restitution should be calculated.
/// @details A coefficient of restitution is calculated from two collider material restitution values (`a` and `b`).
enum class restitution_combine_mode: std::uint8_t
{
	/// Coefficient of restitution is calculated as `(a + b) / 2`.
	average,
	
	/// Coefficient of restitution is calculated as `min(a, b)`.
	/// @note Takes priority over restitution_combine_mode::average
	minimum,
	
	/// Coefficient of restitution is calculated as `a * b`.
	/// @note Takes priority over restitution_combine_mode::average and restitution_combine_mode::minimum.
	multiply,
	
	/// Coefficient of restitution is calculated as `max(a, b)`.
	/// @note Takes priority over restitution_combine_mode::average, restitution_combine_mode::minimum, and restitution_combine_mode::multiply.
	maximum
};

/// Combines two restitution values into a coefficient of restitution.
/// @param a First restitution value.
/// @param b Second restitution value.
/// @param mode Restitution combine mode.
/// @return Coefficient of restitution of @p a and @p b.
[[nodiscard]] float combine_restitution(float a, float b, restitution_combine_mode mode) noexcept;

} // namespace physics

#endif // ANTKEEPER_PHYSICS_RESTITUTION_HPP

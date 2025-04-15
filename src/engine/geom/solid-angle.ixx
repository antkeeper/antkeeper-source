// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.solid_angle;
import engine.math.constants;
import engine.math.functions;

/// Solid angle functions.
export namespace engine::geom::solid_angle
{
	/// Calculates the solid angle of a cone.
	/// @tparam T Real type.
	/// @param theta Apex angle of the cone, in radians.
	/// @return Solid angle of the cone, in steradians.
	template <class T>
	[[nodiscard]] T cone(T theta)
	{
		return math::two_pi<T> * (T{1} - math::cos(theta));
	}
}

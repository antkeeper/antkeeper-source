// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/functions.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/quaternion-type.hpp>

namespace engine::math
{
	/// @name Axis-Angle Representation
	/// @{

	/// Derives a quaternion from an axis-angle representation.
	/// @param axis Axis of rotation.
	/// @param angle Angle of rotation, in radians.
	/// @return Quaternion representing the rotation.
	template <class T>
	[[nodiscard]] inline quat<T> axis_angle_to_quat(const vec3<T>& axis, T angle) noexcept
	{
		return {cos(angle * T{0.5}), axis * sin(angle * T{0.5})};
	}

	/// @}
}

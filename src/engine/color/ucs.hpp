// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>

namespace engine::color
{
	/// @name CIE 1960 UCS color space
	/// @{

	/// Transforms CIE 1960 UCS chromaticity coordinates into the CIE xyY colorspace.
	/// @tparam T Scalar type.
	/// @param uv CIE 1960 UCS chromaticity coordinates.
	/// @param y Luminance or `Y` value of the resulting xyY color.
	/// @return CIE xyY color.
	template <class T>
	[[nodiscard]] constexpr math::vec3<T> ucs_to_xyy(const math::vec2<T>& uv, T y = T{1}) noexcept
	{
		const T d = T{1} / (T{2} * uv[0] - T{8} * uv[1] + T{4});
		return math::vec3<T>{(T{3} * uv[0]) * d, (T{2} * uv[1]) * d, y};
	}

	/// @}
}

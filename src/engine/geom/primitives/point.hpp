// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::geom::inline primitives
{
	/// *n*-dimensional point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	template <class T, usize N>
	using point = math::vector<T, N>;
}

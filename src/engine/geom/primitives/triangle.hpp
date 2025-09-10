// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::geom::inline primitives
{
	/// *n*-dimensional triangle.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	template <class T, usize N>
	struct triangle
	{
		/// Vector type.
		using vector_type = math::vector<T, N>;

		/// First point.
		vector_type a;

		/// Second point.
		vector_type b;

		/// Third point.
		vector_type c;
	};
}

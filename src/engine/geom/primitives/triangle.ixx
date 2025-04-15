// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.triangle;
export import engine.math.vector;
import engine.utility.sized_types;

export namespace engine::geom::inline primitives
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

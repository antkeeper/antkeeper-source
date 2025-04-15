// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.point;
export import engine.math.vector;
import engine.utility.sized_types;

export namespace engine::geom::inline primitives
{
	/// *n*-dimensional point.
	/// @tparam T Real type.
	/// @tparam N Number of dimensions.
	template <class T, usize N>
	using point = math::vector<T, N>;
}

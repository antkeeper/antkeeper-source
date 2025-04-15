// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.line;
export import engine.geom.primitives.hyperplane;

export namespace engine::geom::inline primitives
{
	/// 2-dimensional hyperplane.
	/// @tparam T Real type.
	template <class T>
	using line = hyperplane<T, 2>;
}

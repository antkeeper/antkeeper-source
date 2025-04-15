// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.plane;
export import engine.geom.primitives.hyperplane;

export namespace engine::geom::inline primitives
{
	/// 3-dimensional hyperplane.
	/// @tparam T Real type.
	template <class T>
	using plane = hyperplane<T, 3>;
}

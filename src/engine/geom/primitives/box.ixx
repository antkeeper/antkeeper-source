// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.box;
export import engine.geom.primitives.hyperrectangle;

export namespace engine::geom::inline primitives
{
	/// 3-dimensional hyperrectangle.
	/// @tparam T Real type.
	template <class T>
	using box = hyperrectangle<T, 3>;
}

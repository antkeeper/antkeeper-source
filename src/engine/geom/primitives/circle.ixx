// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.circle;
export import engine.geom.primitives.hypersphere;

export namespace engine::geom::inline primitives
{
	/// 2-dimensional hypersphere.
	/// @tparam T Real type.
	template <class T>
	using circle = hypersphere<T, 2>;
}

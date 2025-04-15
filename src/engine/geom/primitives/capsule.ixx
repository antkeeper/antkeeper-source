// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.geom.primitives.capsule;
export import engine.geom.primitives.hypercapsule;

export namespace engine::geom::inline primitives
{
	/// 3-dimensional hypercapsule.
	/// @tparam T Real type.
	template <class T>
	using capsule = hypercapsule<T, 3>;
}

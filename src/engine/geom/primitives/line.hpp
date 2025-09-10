// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/primitives/hyperplane.hpp>

namespace engine::geom::inline primitives
{
	/// 2-dimensional hyperplane.
	/// @tparam T Real type.
	template <class T>
	using line = hyperplane<T, 2>;
}

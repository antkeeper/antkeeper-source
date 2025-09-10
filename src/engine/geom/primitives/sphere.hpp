// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/geom/primitives/hypersphere.hpp>

namespace engine::geom::inline primitives
{
	/// 3-dimensional hypersphere.
	/// @tparam T Real type.
	template <class T>
	using sphere = hypersphere<T, 3>;
}

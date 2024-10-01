// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_TYPES_HPP
#define ANTKEEPER_MATH_TYPES_HPP

#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/math/se3.hpp>
#include <engine/math/transform.hpp>
#include <engine/math/vector.hpp>

// export module math.types;
// export import math.matrix;
// export import math.quaternion;
// export import math.vector;

namespace math
{
	/// Mathematical types.
	namespace types {}
	
	// Bring math::types into math namespace
	using namespace types;
}

#endif // ANTKEEPER_MATH_TYPES_HPP

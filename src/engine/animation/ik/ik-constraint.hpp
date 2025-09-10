// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/quaternion.hpp>

namespace engine::animation
{
	/// Abstract base class for IK joint constraints.
	class ik_constraint
	{
	public:
		/// Destructs an IK constraint.
		virtual ~ik_constraint() = default;

		/// Solves the constraint.
		/// @param[in,out] q Unit quaternion representing the rotation of a joint.
		virtual void solve(math::fquat& q) = 0;
	};
}

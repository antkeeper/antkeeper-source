// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_IK_CONSTRAINT_HPP
#define ANTKEEPER_ANIMATION_IK_CONSTRAINT_HPP

#include <engine/math/quaternion.hpp>

/**
 * Abstract base class for IK joint constraints.
 */
class ik_constraint
{
public:
	/**
	 * Solves the constraint.
	 *
	 * @param[in,out] q Unit quaternion representing the rotation of a joint.
	 */
	virtual void solve(math::fquat& q) = 0;
};

#endif // ANTKEEPER_ANIMATION_IK_CONSTRAINT_HPP

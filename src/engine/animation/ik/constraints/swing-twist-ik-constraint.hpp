// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_ANIMATION_SWING_TWIST_IK_CONSTRAINT_HPP
#define ANTKEEPER_ANIMATION_SWING_TWIST_IK_CONSTRAINT_HPP

#include <engine/animation/ik/ik-constraint.hpp>
#include <engine/math/constants.hpp>

/// IK constraint with cone-limited swing and angle-limited twist.
class swing_twist_ik_constraint: public ik_constraint
{
public:
	/// Destructs a swing-twist IK constraint.
	~swing_twist_ik_constraint() override = default;
	
	/// Solves the swing-twist IK constraint.
	void solve(math::fquat& q) override;
	
	/// Sets the twist rotation limit.
	/// @param min_angle Minimum twist angle, in radians.
	/// @param max_angle Maximum twist angle, in radians.
	void set_twist_limit(float min_angle, float max_angle);
	
private:
	float m_cos_half_twist_min{ 0};
	float m_sin_half_twist_min{-1};
	float m_cos_half_twist_max{ 0};
	float m_sin_half_twist_max{ 1};
};

#endif // ANTKEEPER_ANIMATION_SWING_TWIST_IK_CONSTRAINT_HPP

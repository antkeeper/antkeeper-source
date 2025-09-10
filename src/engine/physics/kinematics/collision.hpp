// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/physics/kinematics/rigid-body.hpp>
#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>
#include <array>

namespace engine::physics
{
	/// Point of contact between two colliding bodies.
	struct collision_contact
	{
		/// World-space contact point.
		math::fvec3 point{};

		/// Contact normal, pointing from body a to body b.
		math::fvec3 normal{};

		/// Contact penetration depth.
		float depth{};
	};

	/// Collection of contact points between two colliding bodies.
	/// @tparam N Maximum number of contact points.
	template <u8 N>
	struct collision_manifold
	{
		/// First colliding body.
		rigid_body* body_a{};

		/// Second colliding body.
		rigid_body* body_b{};

		/// Set of contact points between body a and body b.
		std::array<collision_contact, N> contacts;

		/// Number of contact points between body a and body b.
		u8 contact_count{};
	};
}

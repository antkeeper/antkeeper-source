// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.orbit.state;
export import engine.math.vector;

export namespace engine::physics::orbit
{
	/// Pair of orbital state Cartesian position (r) and velocity (v) vectors.
	/// @tparam T Scalar type.
	template <class T>
	struct state
	{
		/// Scalar type.
		using scalar_type = T;
	
		/// Vector type.
		using vector_type = math::vec3<T>;
	
		/// Cartesian orbital position vector (r).
		vector_type r;
	
		/// Cartesian orbital velocity vector (v).
		vector_type v;
	};
}

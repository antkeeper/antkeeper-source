// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.orbit.ephemeris;
export import engine.physics.orbit.trajectory;
export import <vector>;

export namespace engine::physics::orbit
{
	/// Table of orbital trajectories.
	/// @tparam t Real type.
	template <class T>
	struct ephemeris
	{
		/// Table of orbital trajectories.
		std::vector<trajectory<T>> trajectories;
	};
}

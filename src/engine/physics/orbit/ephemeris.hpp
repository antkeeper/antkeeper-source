// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_ORBIT_EPHEMERIS_HPP
#define ANTKEEPER_PHYSICS_ORBIT_EPHEMERIS_HPP

#include <engine/physics/orbit/trajectory.hpp>
#include <vector>

namespace physics {
namespace orbit {

/// Table of orbital trajectories.
/// @tparam t Real type.
template <class T>
struct ephemeris
{
	/// Table of orbital trajectories.
	std::vector<trajectory<T>> trajectories;
};

} // namespace orbit
} // namespace physics

#endif // ANTKEEPER_PHYSICS_ORBIT_EPHEMERIS_HPP

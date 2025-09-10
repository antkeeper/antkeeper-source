// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/physics/orbit/trajectory.hpp>
#include <vector>

namespace engine::physics::orbit
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

/*
 * Copyright (C) 2023  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <engine/physics/kinematics/friction.hpp>
#include <algorithm>

namespace physics {

float combine_friction(float a, float b, friction_combine_mode mode) noexcept
{
	switch (mode)
	{
		case friction_combine_mode::average:
			return (a + b) * 0.5f;
		
		case friction_combine_mode::minimum:
			return std::min(a, b);
		
		case friction_combine_mode::multiply:
			return a * b;
		
		case friction_combine_mode::maximum:
			return std::max(a, b);
		
		default:
			break;
	}
	
	return 0.0f;
}

} // namespace physics

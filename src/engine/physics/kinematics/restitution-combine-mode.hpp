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

#ifndef ANTKEEPER_PHYSICS_RESTITUTION_COMBINE_MODE_HPP
#define ANTKEEPER_PHYSICS_RESTITUTION_COMBINE_MODE_HPP

#include <cstdint>

namespace physics {

/**
 * Specifies how coefficients of restitution should be calculated.
 *
 * A coefficient of restitution is calculated from two collider material restitution values (`a` and `b`).
 */
enum class restitution_combine_mode: std::uint8_t
{
	/**
	 * Coefficient of restitution is calculated as `(a + b) / 2`.
	 */
	average,
	
	/**
	 * Coefficient of restitution is calculated as `min(a, b)`.
	 *
	 * @note Takes priority over restitution_combine_mode::average
	 */
	minimum,
	
	/*
	 * Coefficient of restitution is calculated as `a * b`.
	 *
	 * @note Takes priority over restitution_combine_mode::average and restitution_combine_mode::minimum.
	 */
	multiply,
	
	/*
	 * Coefficient of restitution is calculated as `max(a, b)`.
	 *
	 * @note Takes priority over restitution_combine_mode::average, restitution_combine_mode::minimum, and restitution_combine_mode::multiply.
	 */
	maximum
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_RESTITUTION_COMBINE_MODE_HPP

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

#ifndef ANTKEEPER_PHYSICS_FRICTION_HPP
#define ANTKEEPER_PHYSICS_FRICTION_HPP

#include <cstdint>

namespace physics {

/**
 * Specifies how coefficients of friction should be calculated.
 *
 * A coefficient of friction is calculated from two collider material friction values (`a` and `b`).
 */
enum class friction_combine_mode: std::uint8_t
{
	/**
	 * Coefficient of friction is calculated as `(a + b) / 2`.
	 */
	average,
	
	/**
	 * Coefficient of friction is calculated as `min(a, b)`.
	 *
	 * @note Takes priority over friction_combine_mode::average
	 */
	minimum,
	
	/*
	 * Coefficient of friction is calculated as `a * b`.
	 *
	 * @note Takes priority over friction_combine_mode::average and friction_combine_mode::minimum.
	 */
	multiply,
	
	/*
	 * Coefficient of friction is calculated as `max(a, b)`.
	 *
	 * @note Takes priority over friction_combine_mode::average, friction_combine_mode::minimum, and friction_combine_mode::multiply.
	 */
	maximum
};

/**
 * Combines two friction values into a coefficient of friction.
 *
 * @param a First friction value.
 * @param b Second friction value.
 * @param mode Friction combine mode.
 *
 * @return Coefficient of friction of @p a and @p b.
 */
[[nodiscard]] float combine_friction(float a, float b, friction_combine_mode mode) noexcept;

} // namespace physics

#endif // ANTKEEPER_PHYSICS_FRICTION_MODE_HPP

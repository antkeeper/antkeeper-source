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

#ifndef ANTKEEPER_PHYSICS_COLLIDER_HPP
#define ANTKEEPER_PHYSICS_COLLIDER_HPP

#include <engine/physics/kinematics/collider-type.hpp>
#include <engine/physics/kinematics/collider-material.hpp>
#include <cstdint>
#include <memory>

namespace physics {

/**
 * Abstract base class for collision objects.
 */
class collider
{
public:
	/**
	 * Returns the collider type.
	 */
	[[nodiscard]] virtual constexpr collider_type type() const noexcept = 0;
	
	/**
	 * Sets the layer mask of the collider.
	 *
	 * @param mask 32-bit layer mask in which each bit represents a layer with which the collider can interact.
	 */
	inline constexpr void set_layer_mask(std::uint32_t mask) noexcept
	{
		m_layer_mask = mask;
	}
	
	/**
	 * Sets the collider material.
	 */
	inline void set_material(std::shared_ptr<collider_material> material) noexcept
	{
		m_material = material;
	}
	
	/// Returns the layer mask of the collider.
	[[nodiscard]] inline constexpr std::uint32_t get_layer_mask() const noexcept
	{
		return m_layer_mask;
	}
	
	/// Returns the collider material.
	[[nodiscard]] inline constexpr const std::shared_ptr<collider_material>& get_material() const noexcept
	{
		return m_material;
	}
	
private:
	/// Layer mask, in which each bit represents a layer with which the rigid body can interact.
	std::uint32_t m_layer_mask{1};
	
	/// Collider material.
	std::shared_ptr<collider_material> m_material;
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_COLLIDER_HPP

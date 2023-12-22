// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
	/** Destructs a collider. */
	virtual ~collider() = default;
	
	/**
	 * Returns the collider type.
	 */
	[[nodiscard]] virtual constexpr collider_type type() const noexcept = 0;
	
	/**
	 * Sets the layer mask of the collider.
	 *
	 * @param mask 32-bit layer mask in which each set bit represents a layer with which the collider can interact.
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

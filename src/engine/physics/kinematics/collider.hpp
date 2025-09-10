// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/physics/kinematics/friction.hpp>
#include <engine/physics/kinematics/restitution.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>

namespace engine::physics
{
	/// Describes the collision response of a collider.
	class collider_material
	{
	public:
		/// Constructs a collider material.
		/// @param restitution Restitution value.
		/// @param static_friction Static friction value.
		/// @param dynamic_friction Dynamic friction value.
		inline constexpr collider_material(float restitution, float static_friction, float dynamic_friction) noexcept:
			m_restitution{restitution},
			m_static_friction{static_friction},
			m_dynamic_friction{dynamic_friction}
		{
		}

		/// Constructs a collider material.
		constexpr collider_material() noexcept = default;

		/// Sets the of restitution of the material.
		/// @param restitution Restitution value.
		inline constexpr void set_restitution(float restitution) noexcept
		{
			m_restitution = restitution;
		}

		/// Sets the static friction of the material.
		/// @param friction Static friction value.
		inline constexpr void set_static_friction(float friction) noexcept
		{
			m_static_friction = friction;
		}

		/// Sets the dynamic friction of the material.
		/// @param friction Dynamic friction value.
		inline constexpr void set_dynamic_friction(float friction) noexcept
		{
			m_dynamic_friction = friction;
		}

		/// Sets the restitution combine mode of the material.
		/// @param mode Restitution combine mode.
		inline constexpr void set_restitution_combine_mode(restitution_combine_mode mode) noexcept
		{
			m_restitution_combine_mode = mode;
		}

		/// Sets the friction combine mode of the material.
		/// @param mode Friction combine mode.
		inline constexpr void set_friction_combine_mode(friction_combine_mode mode) noexcept
		{
			m_friction_combine_mode = mode;
		}

		/// Sets the density of the material.
		/// @param density Material density.
		inline constexpr void set_density(float density) noexcept
		{
			m_density = density;
		}

		/// Returns the restitution of the material.
		[[nodiscard]] inline constexpr float get_restitution() const noexcept
		{
			return m_restitution;
		}

		/// Returns the static friction of the material.
		[[nodiscard]] inline constexpr float get_static_friction() const noexcept
		{
			return m_static_friction;
		}

		/// Returns the dynamic friction of the material.
		[[nodiscard]] inline constexpr float get_dynamic_friction() const noexcept
		{
			return m_dynamic_friction;
		}

		/// Returns the restitution combine mode.
		[[nodiscard]] inline constexpr auto get_restitution_combine_mode() const noexcept
		{
			return m_restitution_combine_mode;
		}

		/// Returns the friction combine mode.
		[[nodiscard]] inline constexpr auto get_friction_combine_mode() const noexcept
		{
			return m_friction_combine_mode;
		}

		/// Returns the density of the material.
		[[nodiscard]] inline constexpr float get_density() const noexcept
		{
			return m_density;
		}

	private:
		/// Restitution value.
		float m_restitution{};

		/// Static friction value.
		float m_static_friction{};

		/// Dynamic friction value.
		float m_dynamic_friction{};

		/// Restitution combine mode.
		restitution_combine_mode m_restitution_combine_mode{restitution_combine_mode::average};

		/// Friction combine mode.
		friction_combine_mode m_friction_combine_mode{friction_combine_mode::average};

		/// Density.
		float m_density{1.0f};
	};

	/// Collider types.
	enum class collider_type
	{
		/// Plane collider.
		plane,

		/// Sphere collider.
		sphere,

		/// Box collider.
		box,

		/// Capsule collider.
		capsule,

		/// Mesh collider.
		mesh
	};

	/// Abstract base class for collision objects.
	class collider
	{
	public:
		/// Destructs a collider.
		virtual ~collider() = default;

		/// Returns the collider type.
		[[nodiscard]] virtual constexpr collider_type type() const noexcept = 0;

		/// Sets the layer mask of the collider.
		/// @param mask 32-bit layer mask in which each set bit represents a layer with which the collider can interact.
		inline constexpr void set_layer_mask(u32 mask) noexcept
		{
			m_layer_mask = mask;
		}

		/// Sets the collider material.
		/// @param material Collider material.
		inline void set_material(std::shared_ptr<collider_material> material) noexcept
		{
			m_material = material;
		}

		/// Returns the layer mask of the collider.
		[[nodiscard]] inline constexpr u32 get_layer_mask() const noexcept
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
		u32 m_layer_mask{1};

		/// Collider material.
		std::shared_ptr<collider_material> m_material;
	};
}

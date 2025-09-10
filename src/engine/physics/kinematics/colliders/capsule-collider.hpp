// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/physics/kinematics/collider.hpp>
#include <engine/geom/primitives/capsule.hpp>

namespace engine::physics
{
	/// Capsule collision object.
	class capsule_collider: public collider
	{
	public:
		/// Capsule type.
		using capsule_type = engine::geom::capsule<float>;

		[[nodiscard]] inline constexpr collider_type type() const noexcept override
		{
			return collider_type::capsule;
		}

		/// Constructs a capsule collider from a capsule.
		/// @param capsule Capsule shape.
		inline constexpr explicit capsule_collider(const capsule_type& capsule) noexcept:
			m_capsule{capsule}
		{
		}

		/// Constructs a capsule collider.
		/// @param segment Capsule line segment.
		/// @param radius Capsule hemisphere radius.
		inline constexpr capsule_collider(const capsule_type::segment_type& segment, float radius) noexcept:
			m_capsule{segment, radius}
		{
		}

		/// Constructs a capsule collider.
		constexpr capsule_collider() noexcept = default;

		/// Destructs a capsule collider.
		~capsule_collider() override = default;

		/// Sets the collider's capsule.
		/// @param capsule Capsule shape.
		inline constexpr void set_capsule(const capsule_type& capsule) noexcept
		{
			m_capsule = capsule;
		}

		/// Sets the segment of the capsule.
		/// @param segment Capsule segment, in object space.
		inline constexpr void set_segment(const capsule_type::segment_type& segment) noexcept
		{
			m_capsule.segment = segment;
		}

		/// Sets the radius of the capsule hemispheres.
		/// @param radius Capsule hemisphere radius.
		inline constexpr void set_radius(float radius) noexcept
		{
			m_capsule.radius = radius;
		}

		/// Returns the capsule shape.
		[[nodiscard]] inline constexpr const capsule_type& get_capsule() const noexcept
		{
			return m_capsule;
		}

		/// Returns the segment of the capsule, in object space.
		[[nodiscard]] inline constexpr const capsule_type::segment_type& get_segment() const noexcept
		{
			return m_capsule.segment;
		}

		/// Returns the radius of the capsule hemispheres.
		[[nodiscard]] inline constexpr float get_radius() const noexcept
		{
			return m_capsule.radius;
		}

	private:
		capsule_type m_capsule{};
	};
}

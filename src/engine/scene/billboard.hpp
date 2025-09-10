// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/render/material.hpp>
#include <engine/render/operation.hpp>
#include <engine/scene/object.hpp>
#include <engine/math/vector.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <memory>

namespace engine::scene
{
	/// Billboard types.
	enum class billboard_type
	{
		/// Billboard is unaligned.
		flat,

		/// Billboard aligns to face camera.
		spherical,

		/// Billboard rotates about an alignment axis to face camera.
		cylindrical
	};

	/// 2D unit quad with a single material.
	class billboard: public object<billboard>
	{
	public:
		/// Constructs a billboard.
		billboard();
	
		/// Destructs a billboard.
		~billboard() override = default;
	
		void render(render::context& ctx) const override;
	
		/// Sets the billboard material.
		/// @param material Billboard material.
		void set_material(std::shared_ptr<render::material> material);
	
		/// Sets the billboard type.
		/// @param type Billboard type.
		void set_billboard_type(billboard_type type);
	
		/// Sets alignment axis for cylindrical billboards.
		/// @param axis Cylindral billboard aligmment axis.
		inline void set_alignment_axis(const math::fvec3& axis) noexcept
		{
			m_alignment_axis = axis;
		}
	
		[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
		{
			return m_bounds;
		}
	
		[[nodiscard]] inline const auto& get_material() const noexcept
		{
			return m_render_op.material;
		}
	
		[[nodiscard]] inline auto get_billboard_type() const noexcept
		{
			return m_billboard_type;
		}
	
		[[nodiscard]] inline const auto& get_alignment_axis() const noexcept
		{
			return m_alignment_axis;
		}

	private:
		void transformed() override;
	
		std::unique_ptr<gl::vertex_array> m_vertex_array;
		std::unique_ptr<gl::vertex_buffer> m_vertex_buffer;
		mutable render::operation m_render_op;
		aabb_type m_bounds{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
		billboard_type m_billboard_type{billboard_type::flat};
		math::fvec3 m_alignment_axis{0.0f, 1.0f, 0.0f};
	};
}

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

#ifndef ANTKEEPER_SCENE_BILLBOARD_HPP
#define ANTKEEPER_SCENE_BILLBOARD_HPP

#include <engine/scene/object.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/material.hpp>
#include <engine/render/operation.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/scene/billboard-type.hpp>
#include <memory>

namespace scene {

/**
 * 2D unit quad with a single material.
 */
class billboard: public object<billboard>
{
public:
	/// Constructs a billboard.
	billboard();
	
	void render(render::context& ctx) const override;
	
	/**
	 * Sets the billboard material.
	 *
	 * @param material Billboard material.
	 */
	void set_material(std::shared_ptr<render::material> material);
	
	/**
	 * Sets the billboard type.
	 *
	 * @param type Billboard type.
	 */
	void set_billboard_type(billboard_type type);
	
	/**
	 * Sets alignment axis for cylindrical billboards.
	 *
	 * @param axis Cylindral billboard aligmment axis.
	 */
	inline void set_alignment_axis(const math::fvec3& axis) noexcept
	{
		m_alignment_axis = axis;
	}
	
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}
	
	[[nodiscard]] inline std::shared_ptr<render::material> get_material() const noexcept
	{
		return m_render_op.material;
	}
	
	[[nodiscard]] inline billboard_type get_billboard_type() const noexcept
	{
		return m_billboard_type;
	}
	
	[[nodiscard]] inline const math::fvec3& get_alignment_axis() const noexcept
	{
		return m_alignment_axis;
	}

private:
	void transformed() override;
	
	std::unique_ptr<gl::vertex_buffer> m_vbo;
	std::unique_ptr<gl::vertex_array> m_vao;
	mutable render::operation m_render_op;
	aabb_type m_bounds{{-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
	billboard_type m_billboard_type{billboard_type::flat};
	math::fvec3 m_alignment_axis{0.0f, 1.0f, 0.0f};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_BILLBOARD_HPP

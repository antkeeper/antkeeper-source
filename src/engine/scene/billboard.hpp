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
#include <engine/geom/aabb.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/render/material.hpp>
#include <engine/render/operation.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <cstdint>
#include <memory>

namespace scene {

/// Enumerates billboard types.
enum class billboard_type: std::uint8_t
{
	/// No alignment
	flat,
	
	/// Aligns to face camera
	spherical,
	
	/// Rotates about an alignment axis to face camera
	cylindrical
};

/**
 * A 2D unit quad with one material.
 */
class billboard: public object<billboard>
{
public:
	typedef geom::aabb<float> aabb_type;
	
	billboard();
	
	void render(render::context& ctx) const override;

	void set_material(std::shared_ptr<render::material> material);
	
	/// Sets the billboard alignment mode.
	void set_billboard_type(billboard_type type);
	
	/// Sets the axis around which the billboard will be rotated when the alignment is set to billboard_alignment::cylindrical.
	void set_alignment_axis(const float3& axis);
	
	[[nodiscard]] inline virtual const bounding_volume_type& get_local_bounds() const noexcept
	{
		return local_bounds;
	}
	
	[[nodiscard]] inline virtual const bounding_volume_type& get_world_bounds() const noexcept
	{
		return world_bounds;
	}

	[[nodiscard]] inline std::shared_ptr<render::material> get_material() const noexcept
	{
		return render_op.material;
	}
	
	[[nodiscard]] inline billboard_type get_billboard_type() const noexcept
	{
		return type;
	}
	
	[[nodiscard]] inline const float3& get_alignment_axis() const noexcept
	{
		return alignment_axis;
	}
	
	virtual void update_tweens();

private:
	static const aabb_type local_bounds;
	
	virtual void transformed();
	
	std::unique_ptr<gl::vertex_buffer> vbo;
	std::unique_ptr<gl::vertex_array> vao;
	
	mutable render::operation render_op;
	aabb_type world_bounds;
	billboard_type type;
	float3 alignment_axis;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_BILLBOARD_HPP

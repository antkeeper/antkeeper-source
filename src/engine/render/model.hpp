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

#ifndef ANTKEEPER_RENDER_MODEL_HPP
#define ANTKEEPER_RENDER_MODEL_HPP

#include <engine/animation/skeleton.hpp>
#include <engine/geom/aabb.hpp>
#include <engine/gl/drawing-mode.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/render/material.hpp>
#include <engine/utility/hash/fnv1a.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace render {

/**
 * Part of a model which is associated with exactly one material.
 */
struct model_group
{
	hash::fnv1a32_t id;
	gl::drawing_mode drawing_mode;
	std::uint32_t start_index;
	std::uint32_t index_count;
	std::shared_ptr<material> material;
};

/**
 * 
 */
class model
{
public:
	/// AABB type.
	typedef geom::aabb<float> aabb_type;
	
	/**
	 * Constructs a model.
	 */
	model();
	
	/**
	 * Returns the vertex array associated with this model.
	 */
	/// @{
	[[nodiscard]] inline const std::shared_ptr<gl::vertex_array>& get_vertex_array() const noexcept
	{
		return vertex_array;
	}
	[[nodiscard]] inline std::shared_ptr<gl::vertex_array>& get_vertex_array() noexcept
	{
		return vertex_array;
	}
	/// @}
	
	/**
	 * Returns the vertex buffer associated with this model.
	 */
	/// @{
	[[nodiscard]] inline const std::shared_ptr<gl::vertex_buffer>& get_vertex_buffer() const noexcept
	{
		return vertex_buffer;
	}
	[[nodiscard]] inline std::shared_ptr<gl::vertex_buffer>& get_vertex_buffer() noexcept
	{
		return vertex_buffer;
	}
	/// @}
	
	/**
	 * Returns the bounds of the model.
	 */
	/// @{
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept
	{
		return bounds;
	}
	[[nodiscard]] inline aabb_type& get_bounds() noexcept
	{
		return bounds;
	}
	/// @}
	
	/**
	 * Returns the model's model groups.
	 */
	/// @{
	[[nodiscard]] inline const std::vector<model_group>& get_groups() const noexcept
	{
		return groups;
	}
	[[nodiscard]] inline std::vector<model_group>& get_groups() noexcept
	{
		return groups;
	}
	/// @}
	
	/**
	 * Returns the skeleton associated with this model.
	 */
	/// @{
	[[nodiscard]] inline const ::skeleton& get_skeleton() const noexcept
	{
		return skeleton;
	}
	[[nodiscard]] inline ::skeleton& get_skeleton() noexcept
	{
		return skeleton;
	}
	/// @}
	
private:
	std::shared_ptr<gl::vertex_array> vertex_array;
	std::shared_ptr<gl::vertex_buffer> vertex_buffer;
	aabb_type bounds{{0, 0, 0}, {0, 0, 0}};
	std::vector<model_group> groups;
	::skeleton skeleton;
};

} // namespace render

#endif // ANTKEEPER_RENDER_MODEL_HPP

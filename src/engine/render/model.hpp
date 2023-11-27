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
#include <engine/geom/primitives/box.hpp>
#include <engine/gl/primitive-topology.hpp>
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
	hash::fnv1a32_t id{};
	gl::primitive_topology primitive_topology{gl::primitive_topology::triangle_list};
	std::uint32_t first_vertex{};
	std::uint32_t vertex_count{};
	std::shared_ptr<render::material> material;
};

/**
 * 
 */
class model
{
public:
	/// AABB type.
	using aabb_type = geom::box<float>;
	
	/**
	 * Sets the byte offset to the first vertex in the vertex buffer.
	 *
	 * @param offset Byte offset into the vertex buffer.
	 */
	inline void set_vertex_offset(std::size_t offset) noexcept
	{
		m_vertex_offset = offset;
	}
	
	/**
	 * Sets the byte stride between consecutive elements within the vertex buffer.
	 *
	 * @param stride Byte stride between consecutive elements within the vertex buffer.
	 */
	inline void set_vertex_stride(std::size_t stride) noexcept
	{
		m_vertex_stride = stride;
	}
	
	/**
	 * Returns the vertex array associated with this model.
	 */
	/// @{
	[[nodiscard]] inline const std::shared_ptr<gl::vertex_array>& get_vertex_array() const noexcept
	{
		return m_vertex_array;
	}
	[[nodiscard]] inline std::shared_ptr<gl::vertex_array>& get_vertex_array() noexcept
	{
		return m_vertex_array;
	}
	/// @}
	
	/**
	 * Returns the vertex buffer associated with this model.
	 */
	/// @{
	[[nodiscard]] inline const std::shared_ptr<gl::vertex_buffer>& get_vertex_buffer() const noexcept
	{
		return m_vertex_buffer;
	}
	[[nodiscard]] inline std::shared_ptr<gl::vertex_buffer>& get_vertex_buffer() noexcept
	{
		return m_vertex_buffer;
	}
	/// @}
	
	/// Returns the byte offset to the first vertex in the vertex buffer.
	[[nodiscard]] inline constexpr std::size_t get_vertex_offset() const noexcept
	{
		return m_vertex_offset;
	}
	
	/// Returns the byte stride between consecutive elements within the vertex buffer.
	[[nodiscard]] inline constexpr std::size_t get_vertex_stride() const noexcept
	{
		return m_vertex_stride;
	}
	
	/**
	 * Returns the bounds of the model.
	 */
	/// @{
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept
	{
		return m_bounds;
	}
	[[nodiscard]] inline aabb_type& get_bounds() noexcept
	{
		return m_bounds;
	}
	/// @}
	
	/**
	 * Returns the model's model groups.
	 */
	/// @{
	[[nodiscard]] inline const std::vector<model_group>& get_groups() const noexcept
	{
		return m_groups;
	}
	[[nodiscard]] inline std::vector<model_group>& get_groups() noexcept
	{
		return m_groups;
	}
	/// @}
	
	/**
	 * Returns the skeleton associated with this model.
	 */
	/// @{
	[[nodiscard]] inline const ::skeleton& get_skeleton() const noexcept
	{
		return m_skeleton;
	}
	[[nodiscard]] inline ::skeleton& get_skeleton() noexcept
	{
		return m_skeleton;
	}
	/// @}
	
private:
	std::shared_ptr<gl::vertex_array> m_vertex_array;
	std::shared_ptr<gl::vertex_buffer> m_vertex_buffer;
	std::size_t m_vertex_offset{};
	std::size_t m_vertex_stride{};
	aabb_type m_bounds{{0, 0, 0}, {0, 0, 0}};
	std::vector<model_group> m_groups;
	::skeleton m_skeleton;
};

} // namespace render

#endif // ANTKEEPER_RENDER_MODEL_HPP

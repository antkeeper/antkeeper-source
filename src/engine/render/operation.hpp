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

#ifndef ANTKEEPER_RENDER_OPERATION_HPP
#define ANTKEEPER_RENDER_OPERATION_HPP

#include <engine/math/vector.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/primitive-topology.hpp>
#include <engine/render/material.hpp>
#include <cstdint>
#include <memory>
#include <span>

namespace render {

/**
 * Atomic render operation.
 */
struct operation
{
	gl::primitive_topology primitive_topology{gl::primitive_topology::triangle_list};
	const gl::vertex_array* vertex_array{nullptr};
	const gl::vertex_buffer* vertex_buffer{nullptr};
	std::size_t vertex_offset{0};
	std::size_t vertex_stride{0};
	std::uint32_t first_vertex{0};
	std::uint32_t vertex_count{0};
	std::uint32_t first_instance{0};
	std::uint32_t instance_count{1};
	
	std::shared_ptr<render::material> material;
	
	math::fmat4 transform{math::fmat4::identity()};
	float depth{};
	
	std::span<const math::fmat4> matrix_palette{};
	
	std::uint32_t layer_mask{};
};

} // namespace render

#endif // ANTKEEPER_RENDER_OPERATION_HPP

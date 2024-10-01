// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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

/// Atomic render operation.
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
	
	math::fmat4 transform{math::identity<math::fmat4>};
	float depth{};
	
	std::span<const math::fmat4> skinning_matrices{};
	
	std::uint32_t layer_mask{};
};

} // namespace render

#endif // ANTKEEPER_RENDER_OPERATION_HPP

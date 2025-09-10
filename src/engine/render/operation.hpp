// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/math/vector.hpp>
#include <engine/render/material.hpp>
#include <engine/math/matrix.hpp>
#include <engine/gl/vertex-array.hpp>
#include <engine/gl/vertex-buffer.hpp>
#include <engine/gl/primitive-topology.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>
#include <span>

namespace engine::render
{
	/// Atomic render operation.
	struct operation
	{
		gl::primitive_topology primitive_topology{gl::primitive_topology::triangle_list};
		const gl::vertex_array* vertex_array{nullptr};
		const gl::vertex_buffer* vertex_buffer{nullptr};
		usize vertex_offset{0};
		usize vertex_stride{0};
		u32 first_vertex{0};
		u32 vertex_count{0};
		u32 first_instance{0};
		u32 instance_count{1};
		std::shared_ptr<render::material> material;
		math::fmat4 transform{math::identity<math::fmat4>};
		float depth{};
		std::span<const math::fmat4> skinning_matrices{};
		u32 layer_mask{};
	};
}

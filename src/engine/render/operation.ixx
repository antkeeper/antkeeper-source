// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.render.operation;
import engine.math.vector;
import engine.render.material;
import engine.math.matrix;
import engine.gl.vertex_array;
import engine.gl.vertex_buffer;
import engine.gl.primitive_topology;
import engine.utility.sized_types;
export import <memory>;
import <span>;

export namespace engine::render
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

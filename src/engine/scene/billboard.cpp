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

#include <engine/scene/billboard.hpp>
#include <engine/config.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/geom/projection.hpp>

namespace scene {

const typename billboard::aabb_type billboard::local_bounds = {{-1, -1, -1}, {1, 1, 1}};

billboard::billboard():
	world_bounds(local_bounds),
	type(billboard_type::flat),
	alignment_axis(config::global_up)
{
	const float vertex_data[] =
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	const std::size_t vertex_size = 8;
	const std::size_t vertex_stride = sizeof(float) * vertex_size;
	const std::size_t vertex_count = 6;
	
	vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, sizeof(float) * vertex_size * vertex_count, std::as_bytes(std::span{vertex_data}));
	
	std::size_t attribute_offset = 0;
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo.get();
	position_attribute.offset = attribute_offset;
	position_attribute.stride = vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define UV vertex attribute
	gl::vertex_attribute uv_attribute;
	uv_attribute.buffer = vbo.get();
	uv_attribute.offset = attribute_offset;
	uv_attribute.stride = vertex_stride;
	uv_attribute.type = gl::vertex_attribute_type::float_32;
	uv_attribute.components = 2;
	attribute_offset += uv_attribute.components * sizeof(float);
	
	// Define barycentric vertex attribute
	gl::vertex_attribute barycentric_attribute;
	barycentric_attribute.buffer = vbo.get();
	barycentric_attribute.offset = attribute_offset;
	barycentric_attribute.stride = vertex_stride;
	barycentric_attribute.type = gl::vertex_attribute_type::float_32;
	barycentric_attribute.components = 3;
	//attribute_offset += barycentric_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	vao = std::make_unique<gl::vertex_array>();
	vao->bind(render::vertex_attribute::position, position_attribute);
	vao->bind(render::vertex_attribute::uv, uv_attribute);
	vao->bind(render::vertex_attribute::barycentric, barycentric_attribute);
	
	// Init render operation
	render_op.vertex_array = vao.get();
	render_op.drawing_mode = gl::drawing_mode::triangles;
	render_op.start_index = 0;
	render_op.index_count = 6;
}

void billboard::render(render::context& ctx) const
{
	auto transform = get_transform_tween().interpolate(ctx.alpha);
	
	// Align billboard
	if (type == scene::billboard_type::spherical)
	{
		transform.rotation = math::normalize(math::look_rotation(ctx.camera_forward, ctx.camera_up) * transform.rotation);
	}
	else if (type == scene::billboard_type::cylindrical)
	{
		float3 look = math::normalize(geom::project_on_plane(transform.translation - ctx.camera_transform.translation, {0.0f, 0.0f, 0.0f}, alignment_axis));
		float3 right = math::normalize(math::cross(alignment_axis, look));
		look = math::cross(right, alignment_axis);
		float3 up = math::cross(look, right);
		transform.rotation = math::normalize(math::look_rotation(look, up) * transform.rotation);
	}
	
	render_op.transform = math::matrix_cast(transform);
	render_op.depth = ctx.clip_near.signed_distance(transform.translation);
	
	ctx.operations.emplace_back(&render_op);
}

void billboard::set_material(std::shared_ptr<render::material> material)
{
	render_op.material = material;
}

void billboard::set_billboard_type(billboard_type type)
{
	this->type = type;
}

void billboard::set_alignment_axis(const float3& axis)
{
	this->alignment_axis = axis;
}

void billboard::transformed()
{
	world_bounds = aabb_type::transform(local_bounds, get_transform());
}

void billboard::update_tweens()
{
	object_base::update_tweens();
}

} // namespace scene

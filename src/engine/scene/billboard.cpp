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
#include <engine/scene/camera.hpp>

namespace scene {

billboard::billboard()
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
	
	m_vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, sizeof(float) * vertex_size * vertex_count, std::as_bytes(std::span{vertex_data}));
	
	std::size_t attribute_offset = 0;
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = m_vbo.get();
	position_attribute.offset = attribute_offset;
	position_attribute.stride = vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define UV vertex attribute
	gl::vertex_attribute uv_attribute;
	uv_attribute.buffer = m_vbo.get();
	uv_attribute.offset = attribute_offset;
	uv_attribute.stride = vertex_stride;
	uv_attribute.type = gl::vertex_attribute_type::float_32;
	uv_attribute.components = 2;
	attribute_offset += uv_attribute.components * sizeof(float);
	
	// Define barycentric vertex attribute
	gl::vertex_attribute barycentric_attribute;
	barycentric_attribute.buffer = m_vbo.get();
	barycentric_attribute.offset = attribute_offset;
	barycentric_attribute.stride = vertex_stride;
	barycentric_attribute.type = gl::vertex_attribute_type::float_32;
	barycentric_attribute.components = 3;
	//attribute_offset += barycentric_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	m_vao = std::make_unique<gl::vertex_array>();
	m_vao->bind(render::vertex_attribute::position, position_attribute);
	m_vao->bind(render::vertex_attribute::uv, uv_attribute);
	m_vao->bind(render::vertex_attribute::barycentric, barycentric_attribute);
	
	// Init render operation
	m_render_op.vertex_array = m_vao.get();
	m_render_op.drawing_mode = gl::drawing_mode::triangles;
	m_render_op.start_index = 0;
	m_render_op.index_count = 6;
	m_render_op.transform = math::fmat4::identity();
}

void billboard::render(render::context& ctx) const
{
	// Align billboard
	switch (m_billboard_type)
	{
		case scene::billboard_type::spherical:
		{
			auto transform = get_transform();
			
			transform.rotation = math::normalize(math::look_rotation(ctx.camera->get_forward(), ctx.camera->get_up()) * transform.rotation);
			
			m_render_op.transform = transform.matrix();
			
			break;
		}
		
		case scene::billboard_type::cylindrical:
		{
			auto transform = get_transform();
			
			auto look = math::normalize(geom::project_on_plane(transform.translation - ctx.camera->get_translation(), {0.0f, 0.0f, 0.0f}, m_alignment_axis));
			const auto right = math::normalize(math::cross(m_alignment_axis, look));
			look = math::cross(right, m_alignment_axis);
			const auto up = math::cross(look, right);
			transform.rotation = math::normalize(math::look_rotation(look, up) * transform.rotation);
			
			m_render_op.transform = transform.matrix();
			
			break;
		}
		
		case scene::billboard_type::flat:
			break;
		
		default:
			break;
	}
	
	m_render_op.depth = ctx.camera->get_view_frustum().near().distance(get_translation());
	m_render_op.layer_mask = get_layer_mask();
	
	ctx.operations.emplace_back(&m_render_op);
}

void billboard::set_material(std::shared_ptr<render::material> material)
{
	m_render_op.material = material;
}

void billboard::set_billboard_type(billboard_type type)
{
	m_billboard_type = type;
	
	if (m_billboard_type == scene::billboard_type::flat)
	{
		m_render_op.transform = get_transform().matrix();
	}
}

void billboard::transformed()
{
	m_bounds = {get_translation() - get_scale(), get_translation() + get_scale()};
	
	if (m_billboard_type == scene::billboard_type::flat)
	{
		m_render_op.transform = get_transform().matrix();
	}
}

} // namespace scene

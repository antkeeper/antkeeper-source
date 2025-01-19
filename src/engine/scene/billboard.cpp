// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/billboard.hpp>
#include <engine/config.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/geom/projection.hpp>
#include <engine/scene/camera.hpp>

namespace scene {

namespace {
	
	constexpr gl::vertex_input_attribute billboard_vertex_attributes[2] =
	{
		{
			render::vertex_attribute_location::position,
			0,
			gl::format::r32g32_sfloat,
			0
		},
		{
			render::vertex_attribute_location::uv,
			0,
			gl::format::r32g32_sfloat,
			2 * sizeof(float)
		}
	};
	
	constexpr float billboard_vertex_data[] =
	{
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f, 0.0f
	};
	
	constexpr std::size_t billboard_vertex_stride = 4 * sizeof(float);
}

billboard::billboard()
{
	// Construct vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>(billboard_vertex_attributes);
	
	// Construct vertex buffer
	m_vertex_buffer = std::make_unique<gl::vertex_buffer>
	(
		gl::buffer_usage::static_draw,
		std::as_bytes(std::span{billboard_vertex_data})
	);
	
	// Init render operation
	m_render_op.primitive_topology = gl::primitive_topology::triangle_strip;
	m_render_op.vertex_array = m_vertex_array.get();
	m_render_op.vertex_buffer = m_vertex_buffer.get();
	m_render_op.vertex_offset = 0;
	m_render_op.vertex_stride = billboard_vertex_stride;
	m_render_op.first_vertex = 0;
	m_render_op.vertex_count = 4;
	m_render_op.first_instance = 0;
	m_render_op.instance_count = 1;
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

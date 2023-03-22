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

#include <engine/scene/text.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <engine/type/unicode/convert.hpp>
#include <engine/scene/camera.hpp>
#include <cstddef>

namespace scene {

text::text()
{
	// Allocate VBO and VAO
	m_vbo = std::make_unique<gl::vertex_buffer>();
	m_vao = std::make_unique<gl::vertex_array>();
	
	// Calculate vertex stride
	m_vertex_stride = (3 + 2 + 4) * sizeof(float);
	
	// Init vertex attribute offset
	std::size_t attribute_offset = 0;
	
	// Define vertex position attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = m_vbo.get();
	position_attribute.offset = attribute_offset;
	position_attribute.stride = m_vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define vertex UV attribute
	gl::vertex_attribute uv_attribute;
	uv_attribute.buffer = m_vbo.get();
	uv_attribute.offset = attribute_offset;
	uv_attribute.stride = m_vertex_stride;
	uv_attribute.type = gl::vertex_attribute_type::float_32;
	uv_attribute.components = 2;
	attribute_offset += uv_attribute.components * sizeof(float);
	
	// Define vertex color attribute
	gl::vertex_attribute color_attribute;
	color_attribute.buffer = m_vbo.get();
	color_attribute.offset = attribute_offset;
	color_attribute.stride = m_vertex_stride;
	color_attribute.type = gl::vertex_attribute_type::float_32;
	color_attribute.components = 4;
	//attribute_offset += color_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	m_vao->bind(render::vertex_attribute::position, position_attribute);
	m_vao->bind(render::vertex_attribute::uv, uv_attribute);
	m_vao->bind(render::vertex_attribute::color, color_attribute);
	
	// Init render operation
	m_render_op.vertex_array = m_vao.get();
	m_render_op.drawing_mode = gl::drawing_mode::triangles;
}

void text::render(render::context& ctx) const
{
	if (m_vertex_count)
	{
		m_render_op.depth = ctx.camera->get_view_frustum().near().distance(get_translation());
		ctx.operations.push_back(&m_render_op);
	}
}

void text::refresh()
{
	update_content();
}

void text::set_material(std::shared_ptr<render::material> material)
{
	m_render_op.material = material;
}

void text::set_font(const type::bitmap_font* font)
{
	if (m_font != font)
	{
		m_font = font;
		update_content();
	}
}

void text::set_direction(type::text_direction direction)
{
	if (m_direction != direction)
	{
		m_direction = direction;
		update_content();
	}
}

void text::set_content(const std::string& content)
{
	if (m_content_u8 != content)
	{
		m_content_u8 = content;
		m_content_u32 = type::unicode::u32(m_content_u8);
		update_content();
	}
}

void text::set_color(const float4& color)
{
	m_color = color;
	update_color();
}

void text::transformed()
{
	// Naive algorithm: transform each corner of the AABB
	m_world_bounds.min = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
	m_world_bounds.max = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
	for (std::size_t i = 0; i < 8; ++i)
	{
		m_world_bounds.extend(get_transform() * m_local_bounds.corner(i));
	}
	
	m_render_op.transform = get_transform().matrix();
}

void text::update_content()
{
	// If no valid font or no text, clear vertex count
	if (!m_font || m_content_u32.empty())
	{
		m_vertex_count = 0;
		m_render_op.index_count = 0;
		m_local_bounds = {{0, 0, 0}, {0, 0, 0}};
		transformed();
		return;
	}
	
	// Calculate new vertex count and minimum vertex buffer size
	std::size_t vertex_count = m_content_u32.length() * 6;
	std::size_t min_vertex_buffer_size = vertex_count * m_vertex_stride;
	
	// Expand vertex data buffer to accommodate vertices
	if (m_vertex_data.size() < min_vertex_buffer_size)
	{
		m_vertex_data.resize(min_vertex_buffer_size);
	}
	
	// Get font metrics and bitmap
	const type::font_metrics& font_metrics = m_font->get_font_metrics();
	const image& font_bitmap = m_font->get_bitmap();
	
	// Init pen position
	float2 pen_position = {0.0f, 0.0f};
	
	// Reset local-space bounds
	m_local_bounds.min = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 0.0f};
	m_local_bounds.max = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), 0.0f};
	
	// Generate vertex data
	char32_t previous_code = 0;
	float* v = reinterpret_cast<float*>(m_vertex_data.data());
	for (char32_t code: m_content_u32)
	{
		// Apply kerning
		if (previous_code)
		{
			pen_position.x() += m_font->get_kerning(previous_code, code).x();
		}
		
		if (m_font->contains(code))
		{
			// Get glyph
			const type::bitmap_glyph& glyph = m_font->get_glyph(code);
			
			// Calculate vertex positions
			float2 positions[6];
			positions[0] = pen_position + glyph.metrics.horizontal_bearing;
			positions[1] = {positions[0].x(), positions[0].y() - glyph.metrics.height};
			positions[2] = {positions[0].x() + glyph.metrics.width, positions[1].y()};
			positions[3] = {positions[2].x(), positions[0].y()};
			positions[4] = positions[0];
			positions[5] = positions[2];
			
			// Calculate vertex UVs
			float2 uvs[6];
			uvs[0] = {static_cast<float>(glyph.position.x()), static_cast<float>(glyph.position.y())};
			uvs[1] = {uvs[0].x(), uvs[0].y() + glyph.metrics.height};
			uvs[2] = {uvs[0].x() + glyph.metrics.width, uvs[1].y()};
			uvs[3] = {uvs[2].x(), uvs[0].y()};
			uvs[4] = uvs[0];
			uvs[5] = uvs[2];
			
			for (int i = 0; i < 6; ++i)
			{
				// Round positions
				positions[i].x() = std::round(positions[i].x());
				positions[i].y() = std::round(positions[i].y());
				
				// Normalize UVs
				uvs[i].x() = uvs[i].x() / static_cast<float>(font_bitmap.width());
				uvs[i].y() = uvs[i].y() / static_cast<float>(font_bitmap.height());
			}
			
			// Add vertex to vertex data buffer
			for (int i = 0; i < 6; ++i)
			{
				*(v++) = positions[i].x();
				*(v++) = positions[i].y();
				*(v++) = 0.0f;
				*(v++) = uvs[i].x();
				*(v++) = uvs[i].y();
				*(v++) = m_color[0];
				*(v++) = m_color[1];
				*(v++) = m_color[2];
				*(v++) = m_color[3];
			}
			
			// Advance pen position
			pen_position.x() += glyph.metrics.horizontal_advance;
			
			// Update local-space bounds
			for (int i = 0; i < 4; ++i)
			{
				const float2& position = positions[i];
				for (int j = 0; j < 2; ++j)
				{
					m_local_bounds.min[j] = std::min<float>(m_local_bounds.min[j], position[j]);
					m_local_bounds.max[j] = std::max<float>(m_local_bounds.max[j], position[j]);
				}
			}
		}
		else
		{
			// Glyph not in font, zero vertex data
			for (std::size_t i = 0; i < (6 * 9); ++i)
			{
				*(v++) = 0.0f;
			}
		}
		
		// Handle newlines
		if (code == static_cast<char32_t>('\n'))
		{
			pen_position.x() = 0.0f;
			pen_position.y() -= font_metrics.linegap;
		}
		
		// Update previous UTF-32 character code
		previous_code = code;
	}
	
	// Resize VBO, if necessary, and upload vertex data
	if (vertex_count > m_vertex_count)
	{
		m_vbo->resize(min_vertex_buffer_size, m_vertex_data);
	}
	else
	{
		m_vbo->write({m_vertex_data.data(), min_vertex_buffer_size});
	}
	
	// Update vertex count
	m_vertex_count = vertex_count;
	m_render_op.index_count = vertex_count;
	
	// Update world-space bounds
	transformed();
}

void text::update_color()
{
	float* v = reinterpret_cast<float*>(m_vertex_data.data());
	for (std::size_t i = 0; i < m_vertex_count; ++i)
	{
		// Skip vertex position (vec3) and vertex UV (vec2)
		v += (3 + 2);
		
		// Update vertex color
		*(v++) = m_color[0];
		*(v++) = m_color[1];
		*(v++) = m_color[2];
		*(v++) = m_color[3];
	}
	
	// Update VBO
	m_vbo->write({m_vertex_data.data(), m_vertex_count * m_vertex_stride});
}

} // namespace scene

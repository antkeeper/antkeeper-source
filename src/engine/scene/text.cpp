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
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/type/unicode/convert.hpp>
#include <engine/scene/camera.hpp>
#include <engine/debug/log.hpp>
#include <cstddef>

namespace scene {

namespace {
	
	/// Text vertex attributes.
	constexpr gl::vertex_input_attribute text_vertex_attributes[3] =
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
		},
		{
			render::vertex_attribute_location::color,
			0,
			gl::format::r32g32b32a32_sfloat,
			4 * sizeof(float)
		}
	};
	
	/// Text vertex stride.
	constexpr std::size_t text_vertex_stride = (2 + 2 + 4) * sizeof(float);
}

text::text()
{
	// Construct vertex array
	m_vertex_array = std::make_unique<gl::vertex_array>(text_vertex_attributes);
	
	// Construct empty vertex buffer
	m_vertex_buffer = std::make_unique<gl::vertex_buffer>();
	
	// Init render operation
	m_render_op.primitive_topology = gl::primitive_topology::triangle_list;
	m_render_op.vertex_array = m_vertex_array.get();
	m_render_op.vertex_buffer = m_vertex_buffer.get();
	m_render_op.vertex_offset = 0;
	m_render_op.vertex_stride = text_vertex_stride;
	m_render_op.first_vertex = 0;
	m_render_op.vertex_count = 0;
	m_render_op.first_instance = 0;
	m_render_op.instance_count = 1;
}

void text::render(render::context& ctx) const
{
	if (m_render_op.vertex_count)
	{
		m_render_op.depth = ctx.camera->get_view_frustum().near().distance(get_translation());
		m_render_op.layer_mask = get_layer_mask();
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

void text::set_color(const math::fvec4& color)
{
	m_color = color;
	update_color();
}

void text::transformed()
{
	// Naive algorithm: transform each corner of the AABB
	m_world_bounds = {math::fvec3::infinity(), -math::fvec3::infinity()};
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
		m_render_op.vertex_count = 0;
		m_local_bounds = {{0, 0, 0}, {0, 0, 0}};
		transformed();
		return;
	}
	
	// Reserve vertex data
	auto vbo_min_size = m_content_u32.length() * text_vertex_stride * 6;
	if (m_vertex_data.size() < vbo_min_size)
	{
		m_vertex_data.resize(vbo_min_size);
	}
	
	std::uint32_t visible_character_count = static_cast<std::uint32_t>(m_content_u32.length());
	
	// Get font metrics and texture
	const type::font_metrics& font_metrics = m_font->get_font_metrics();
	const auto& font_texture = m_font->get_texture();
	const auto& font_texture_dimensions = font_texture->get_image_view()->get_image()->get_dimensions();
	
	// Init pen position
	math::fvec2 pen_position = {0.0f, 0.0f};
	
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
		
		// Get glyph
		const type::bitmap_glyph* glyph = m_font->get_glyph(code);
		if (glyph)
		{
			// Calculate vertex positions
			math::fvec2 positions[6];
			positions[0] = pen_position + glyph->metrics.horizontal_bearing;
			positions[1] = {positions[0].x(), positions[0].y() - glyph->metrics.height};
			positions[2] = {positions[0].x() + glyph->metrics.width, positions[1].y()};
			positions[3] = {positions[2].x(), positions[0].y()};
			positions[4] = positions[0];
			positions[5] = positions[2];
			
			// Calculate vertex UVs
			math::fvec2 uvs[6];
			uvs[0] = {static_cast<float>(glyph->position.x()), static_cast<float>(glyph->position.y())};
			uvs[1] = {uvs[0].x(), uvs[0].y() + glyph->metrics.height};
			uvs[2] = {uvs[0].x() + glyph->metrics.width, uvs[1].y()};
			uvs[3] = {uvs[2].x(), uvs[0].y()};
			uvs[4] = uvs[0];
			uvs[5] = uvs[2];
			
			for (int i = 0; i < 6; ++i)
			{
				// Round positions
				positions[i].x() = std::round(positions[i].x());
				positions[i].y() = std::round(positions[i].y());
				
				// Normalize UVs
				uvs[i].x() = uvs[i].x() / static_cast<float>(font_texture_dimensions[0]);
				uvs[i].y() = uvs[i].y() / static_cast<float>(font_texture_dimensions[1]);
			}
			
			// Add vertex to vertex data buffer
			for (int i = 0; i < 6; ++i)
			{
				*(v++) = positions[i].x();
				*(v++) = positions[i].y();
				*(v++) = uvs[i].x();
				*(v++) = uvs[i].y();
				*(v++) = m_color[0];
				*(v++) = m_color[1];
				*(v++) = m_color[2];
				*(v++) = m_color[3];
			}
			
			// Advance pen position
			pen_position.x() += glyph->metrics.horizontal_advance;
			
			// Update local-space bounds
			for (int i = 0; i < 4; ++i)
			{
				const math::fvec2& position = positions[i];
				for (int j = 0; j < 2; ++j)
				{
					m_local_bounds.min[j] = std::min<float>(m_local_bounds.min[j], position[j]);
					m_local_bounds.max[j] = std::max<float>(m_local_bounds.max[j], position[j]);
				}
			}
		}
		else
		{
			--visible_character_count;
		}
		
		// Handle newlines
		if (code == U'\n')
		{
			pen_position.x() = 0.0f;
			pen_position.y() -= font_metrics.linegap;
		}
		
		// Update previous UTF-32 character code
		previous_code = code;
	}
	
	// Adjust min VBO size
	vbo_min_size = visible_character_count * text_vertex_stride * 6;
	
	// Upload vertex data to VBO, growing VBO size if necessary
	if (vbo_min_size > m_vertex_buffer->size())
	{
		m_vertex_buffer->resize(vbo_min_size, {m_vertex_data.data(), vbo_min_size});
	}
	else
	{
		m_vertex_buffer->write({m_vertex_data.data(), vbo_min_size});
	}
	
	// Update render op
	m_render_op.vertex_count = visible_character_count * 6;
	
	// Update world-space bounds
	transformed();
}

void text::update_color()
{
	std::byte* v = m_vertex_data.data();
	
	// Skip position UV
	v += (2 + 2) * sizeof(float);
	
	for (std::size_t i = 0; i < m_render_op.vertex_count; ++i)
	{
		// Update vertex color
		std::memcpy(v, m_color.data(), sizeof(float) * 4);
		
		v += text_vertex_stride;
	}
	
	// Update VBO
	m_vertex_buffer->write({m_vertex_data.data(), m_render_op.vertex_count * text_vertex_stride});
}

} // namespace scene

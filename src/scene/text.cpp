/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "scene/text.hpp"
#include "render/vertex-attribute.hpp"
#include "math/vector-operators.hpp"
#include "type/unicode/convert.hpp"
#include <cstddef>

namespace scene {

text::text():
	local_bounds{{0, 0, 0}, {0, 0, 0}},
	world_bounds{{0, 0, 0}, {0, 0, 0}},
	material(nullptr),
	font(nullptr),
	direction(type::text_direction::ltr),
	content_u8(std::string()),
	content_u32(std::u32string()),
	color({0.0f, 0.0f, 0.0f, 1.0f}),
	vertex_stride(0),
	vertex_count(0),
	vao(nullptr),
	vbo(nullptr)
{
	// Allocate VBO and VAO
	vbo = new gl::vertex_buffer(0, nullptr, gl::buffer_usage::static_draw);
	vao = new gl::vertex_array();
	
	// Calculate vertex stride
	vertex_stride = (3 + 2 + 4) * sizeof(float);
	
	// Init vertex attribute offset
	std::size_t attribute_offset = 0;
	
	// Define vertex position attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo;
	position_attribute.offset = attribute_offset;
	position_attribute.stride = vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define vertex UV attribute
	gl::vertex_attribute uv_attribute;
	uv_attribute.buffer = vbo;
	uv_attribute.offset = attribute_offset;
	uv_attribute.stride = vertex_stride;
	uv_attribute.type = gl::vertex_attribute_type::float_32;
	uv_attribute.components = 2;
	attribute_offset += uv_attribute.components * sizeof(float);
	
	// Define vertex color attribute
	gl::vertex_attribute color_attribute;
	color_attribute.buffer = vbo;
	color_attribute.offset = attribute_offset;
	color_attribute.stride = vertex_stride;
	color_attribute.type = gl::vertex_attribute_type::float_32;
	color_attribute.components = 4;
	attribute_offset += color_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	vao->bind(render::vertex_attribute::position, position_attribute);
	vao->bind(render::vertex_attribute::uv, uv_attribute);
	vao->bind(render::vertex_attribute::color, color_attribute);
	
	// Init render operation
	render_op.material = nullptr;
	render_op.bone_count = 0;
	render_op.skinning_palette = nullptr;
	render_op.vertex_array = vao;
	render_op.drawing_mode = gl::drawing_mode::triangles;
	render_op.start_index = 0;
	render_op.index_count = 0;
	render_op.instance_count = 0;
}

text::~text()
{
	// Free VAO and VBO
	delete vao;
	delete vbo;
}

void text::render(const render::context& ctx, render::queue& queue) const
{
	if (!vertex_count)
		return;
	
	render_op.transform = math::matrix_cast(get_transform_tween().interpolate(ctx.alpha));
	render_op.depth = ctx.clip_near.signed_distance(math::resize<3>(render_op.transform[3]));
	queue.push_back(render_op);
}

void text::refresh()
{
	update_content();
}

void text::set_material(render::material* material)
{
	this->material = material;
	render_op.material = material;
}

void text::set_font(const type::bitmap_font* font)
{
	if (this->font != font)
	{
		this->font = font;

		// Update text in VBO
		update_content();
	}
}

void text::set_direction(type::text_direction direction)
{
	if (this->direction != direction)
	{
		this->direction = direction;
		
		// Update text in VBO
		update_content();
	}
}

void text::set_content(const std::string& content)
{
	// If content has changed
	if (content_u8 != content)
	{
		// Update UTF-8 content
		content_u8 = content;
		
		// Convert UTF-8 content to UTF-32
		content_u32 = type::unicode::u32(content_u8);
		
		// Update text in VBO
		update_content();
	}
}

void text::set_color(const float4& color)
{
	this->color = color;
	
	// Update color in VBO
	update_color();
}

void text::transformed()
{
	world_bounds = aabb_type::transform(local_bounds, get_transform());
}

void text::update_tweens()
{
	object_base::update_tweens();
	if (material)
	{
		material->update_tweens();
	}
}

void text::update_content()
{
	// If no valid font or no text, clear vertex count
	if (!font || content_u32.empty())
	{
		vertex_count = 0;
		render_op.index_count = vertex_count;
		local_bounds = {{0, 0, 0}, {0, 0, 0}};
		transformed();
		return;
	}
	
	// Calculate new vertex count and minimum vertex buffer size
	std::size_t vertex_count = content_u32.length() * 6;
	std::size_t min_vertex_buffer_size = vertex_count * vertex_stride;
	
	// Expand vertex data buffer to accommodate vertices
	if (vertex_data.size() < min_vertex_buffer_size)
		vertex_data.resize(min_vertex_buffer_size);
	
	// Get font metrics and bitmap
	const type::font_metrics& font_metrics = font->get_font_metrics();
	const image& font_bitmap = font->get_bitmap();
	
	// Init pen position
	float2 pen_position = {0.0f, 0.0f};
	
	// Reset local-space bounds
	local_bounds = {{0, 0, 0}, {0, 0, 0}};
	
	// Generate vertex data
	char32_t previous_code = 0;
	float* v = reinterpret_cast<float*>(vertex_data.data());
	for (char32_t code: content_u32)
	{
		// Apply kerning
		if (previous_code)
		{
			pen_position.x += font->get_kerning(previous_code, code).x;
		}
		
		if (font->contains(code))
		{
			// Get glyph
			const type::bitmap_glyph& glyph = font->get_glyph(code);
			
			// Calculate vertex positions
			float2 positions[6];
			positions[0] = pen_position + glyph.metrics.horizontal_bearing;
			positions[1] = {positions[0].x, positions[0].y - glyph.metrics.height};
			positions[2] = {positions[0].x + glyph.metrics.width, positions[1].y};
			positions[3] = {positions[2].x, positions[0].y};
			positions[4] = positions[0];
			positions[5] = positions[2];
			
			// Calculate vertex UVs
			float2 uvs[6];
			uvs[0] = {static_cast<float>(glyph.position.x), static_cast<float>(glyph.position.y)};
			uvs[1] = {uvs[0].x, uvs[0].y + glyph.metrics.height};
			uvs[2] = {uvs[0].x + glyph.metrics.width, uvs[1].y};
			uvs[3] = {uvs[2].x, uvs[0].y};
			uvs[4] = uvs[0];
			uvs[5] = uvs[2];
			
			for (int i = 0; i < 6; ++i)
			{
				// Round positions
				positions[i].x = std::round(positions[i].x);
				positions[i].y = std::round(positions[i].y);
				
				// Normalize UVs
				uvs[i].x = uvs[i].x / static_cast<float>(font_bitmap.get_width());
				uvs[i].y = uvs[i].y / static_cast<float>(font_bitmap.get_height());
			}
			
			// Add vertex to vertex data buffer
			for (int i = 0; i < 6; ++i)
			{
				*(v++) = positions[i].x;
				*(v++) = positions[i].y;
				*(v++) = 0.0f;
				*(v++) = uvs[i].x;
				*(v++) = uvs[i].y;
				*(v++) = color.x;
				*(v++) = color.y;
				*(v++) = color.z;
				*(v++) = color.w;
			}
			
			// Advance pen position
			pen_position.x += glyph.metrics.horizontal_advance;
			
			// Update local-space bounds
			for (int i = 0; i < 4; ++i)
			{
				const float2& position = positions[i];
				for (int j = 0; j < 2; ++j)
				{
					local_bounds.min_point[j] = std::min<float>(local_bounds.min_point[j], position[j]);
					local_bounds.max_point[j] = std::max<float>(local_bounds.max_point[j], position[j]);
				}
			}
		}
		else
		{
			// Glyph not in font, zero vertex data
			for (std::size_t i = 0; i < (6 * 9); ++i)
				*(v++) = 0.0f;
		}
		
		// Handle newlines
		if (code == static_cast<char32_t>('\n'))
		{
			pen_position.x = 0.0f;
			pen_position.y -= font_metrics.linegap;
		}
		
		// Update previous UTF-32 character code
		previous_code = code;
	}
	
	// Resize VBO, if necessary, and upload vertex data
	if (vertex_count > this->vertex_count)
	{
		this->vertex_count = vertex_count;
		vbo->resize(min_vertex_buffer_size, vertex_data.data());
	}
	else
	{
		vbo->write(0, min_vertex_buffer_size, vertex_data.data());
	}
	
	// Update vertex count
	this->vertex_count = vertex_count;
	render_op.index_count = vertex_count;
	
	// Update world-space bounds
	transformed();
}

void text::update_color()
{
	float* v = reinterpret_cast<float*>(vertex_data.data());
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		// Skip vertex position (vec3) and vertex UV (vec2)
		v += (3 + 2);
		
		// Update vertex color
		*(v++) = color.x;
		*(v++) = color.y;
		*(v++) = color.z;
		*(v++) = color.w;
	}
	
	// Update VBO
	vbo->write(0, vertex_count * vertex_stride, vertex_data.data());
}

} // namespace scene

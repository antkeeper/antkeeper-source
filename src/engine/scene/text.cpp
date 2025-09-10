// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/scene/text.hpp>
#include <engine/scene/camera.hpp>
#include <engine/debug/log.hpp>
#include <engine/type/unicode.hpp>
#include <engine/math/constants.hpp>
#include <engine/math/functions.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/render/context.hpp>
#include <engine/gl/vertex-input-attribute.hpp>
#include <engine/utility/sized-types.hpp>

namespace engine::scene
{
	namespace
	{
		// Text vertex attributes.
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

		// Floating-point elements per text vertex
		static const usize floats_per_text_vertex = 2 + 2 + 4;

		// Text vertex byte stride.
		static const usize text_vertex_stride = floats_per_text_vertex * sizeof(float);
	}

	text::text()
	{
		// Construct vertex array
		m_vertex_array = std::make_unique<gl::vertex_array>(text_vertex_attributes);

		// Construct empty vertex buffer
		m_vertex_buffer = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw);

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

	void text::set_font(std::shared_ptr<type::font> font)
	{
		if (m_font != font)
		{
			m_font = std::move(font);

			if (m_font)
			{
				// Update character UV coordinates each time font texture is resized
				m_font_texture_resized_subscription = m_font->get_texture_resized_channel().subscribe
				(
					[&](const auto&)
					{
						update_uvs();
					}
				);
			}
			else
			{
				m_font_texture_resized_subscription.reset();
			}

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

	void text::set_content(std::string_view content)
	{
		if (m_content_u8 != content)
		{
			m_content_u8 = content;
			m_content_u32 = type::to_utf32(m_content_u8);
			update_content();
		}
	}

	void text::set_color(const math::fvec4& color)
	{
		m_color = color;
		update_colors();
	}

	void text::transformed()
	{
		// Naive algorithm: transform each corner of the AABB
		m_world_bounds = {math::inf<math::fvec3>, -math::inf<math::fvec3>};
		for (usize i = 0; i < 8; ++i)
		{
			m_world_bounds.extend(get_transform() * m_local_bounds.corner(i));
		}

		m_render_op.transform = get_transform().matrix();
	}

	void text::update_uvs()
	{
		if (!m_font || m_content_u32.empty() || m_vertex_data.size() < m_content_u32.length() * 6 * 8)
		{
			return;
		}

		// Determine scale factor for texture coordinates
		const auto& texture_dimensions = m_font->get_texture()->get_image_view()->get_image()->get_dimensions();
		const auto uv_scale = math::fvec2
		{
			1.0f / static_cast<float>(texture_dimensions[0]),
			1.0f / static_cast<float>(texture_dimensions[1])
		};

		// Update texture coordinates
		float* v = m_vertex_data.data() + 2;
		for (auto code : m_content_u32)
		{
			if (!is_visible(code))
			{
				continue;
			}

			// Get glyph from glyph cache
			const auto glyph = m_font->get_cached_glyph(code);

			if (glyph)
			{
				// Glyph found, update glyph UVs
				math::fvec2 uvs[6];
				uvs[0] = {static_cast<float>(glyph->bitmap_position[0]), static_cast<float>(glyph->bitmap_position[1])};
				uvs[1] = {uvs[0].x(), uvs[0].y() + glyph->bitmap_dimensions[1]};
				uvs[2] = {uvs[0].x() + glyph->bitmap_dimensions[0], uvs[1].y()};
				uvs[3] = {uvs[2].x(), uvs[0].y()};
				uvs[4] = uvs[0];
				uvs[5] = uvs[2];

				for (int i = 0; i < 6; ++i)
				{
					*(v++) = uvs[i].x() * uv_scale[0];
					*(v++) = uvs[i].y() * uv_scale[1];
					v += 6;
				}
			}
			else
			{
				// Glyph not yet cached, skip it
				v += 8 * 6;
			}
		}

		update_vertex_buffer();
	}

	void text::update_colors()
	{
		if (m_content_u32.empty() || m_vertex_data.size() < m_content_u32.length() * 6 * 8)
		{
			return;
		}

		float* v = m_vertex_data.data() + 4;
		for (auto code : m_content_u32)
		{
			if (!is_visible(code))
			{
				continue;
			}

			for (int i = 0; i < 6; ++i)
			{
				*(v++) = m_color[0];
				*(v++) = m_color[1];
				*(v++) = m_color[2];
				*(v++) = m_color[3];
				v += 4;
			}
		}

		update_vertex_buffer();
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

		// Cache glyphs
		m_font->cache_glyphs(m_content_u32);

		// Determine maximum required size of vertex buffer, in bytes
		const auto max_vertex_count = m_content_u32.length() * 6;
		const auto max_vertex_buffer_size = max_vertex_count * text_vertex_stride;

		// Reserve vertex data
		if (m_vertex_data.size() < max_vertex_buffer_size / sizeof(float))
		{
			m_vertex_data.resize(max_vertex_buffer_size / sizeof(float));
		}

		// Get font metrics and texture
		const auto& font_metrics = m_font->get_metrics();

		// Determine scale factor for texture coordinates
		const auto& texture_dimensions = m_font->get_texture()->get_image_view()->get_image()->get_dimensions();
		const auto uv_scale = math::fvec2
		{
			1.0f / static_cast<float>(texture_dimensions[0]),
			1.0f / static_cast<float>(texture_dimensions[1])
		};

		// Init pen position
		math::fvec2 pen_position = {0.0f, 0.0f};

		// Reset local-space bounds
		m_local_bounds.min = {std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 0.0f};
		m_local_bounds.max = {-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), 0.0f};

		// Generate vertex data
		m_render_op.vertex_count = 0;
		char32_t previous_code = 0;
		float* v = m_vertex_data.data();
		for (char32_t code : m_content_u32)
		{
			// Get glyph from character code
			const auto& glyph = *m_font->get_cached_glyph(code);

			// Apply kerning
			if (previous_code)
			{
				pen_position.x() += m_font->get_kerning(previous_code, code)[0];
			}

			// Calculate vertex positions
			math::fvec2 positions[6];
			positions[0] = {pen_position[0] + glyph.horizontal_bearings[0], pen_position[1] + glyph.horizontal_bearings[1]};
			positions[1] = {positions[0].x(), positions[0].y() - glyph.bitmap_dimensions[1]};
			positions[2] = {positions[0].x() + glyph.bitmap_dimensions[0], positions[1].y()};
			positions[3] = {positions[2].x(), positions[0].y()};
			positions[4] = positions[0];
			positions[5] = positions[2];

			// Calculate vertex UVs
			math::fvec2 uvs[6];
			uvs[0] = {static_cast<float>(glyph.bitmap_position[0]), static_cast<float>(glyph.bitmap_position[1])};
			uvs[1] = {uvs[0].x(), uvs[0].y() + glyph.bitmap_dimensions[1]};
			uvs[2] = {uvs[0].x() + glyph.bitmap_dimensions[0], uvs[1].y()};
			uvs[3] = {uvs[2].x(), uvs[0].y()};
			uvs[4] = uvs[0];
			uvs[5] = uvs[2];

			for (int i = 0; i < 6; ++i)
			{
				// Round positions
				positions[i].x() = math::round(positions[i].x());
				positions[i].y() = math::round(positions[i].y());

				// Normalize UVs
				uvs[i] *= uv_scale;
			}

			// Advance pen position
			pen_position.x() += glyph.horizontal_advance;

			// Handle newlines
			if (code == U'\n')
			{
				pen_position.x() = 0.0f;
				pen_position.y() -= font_metrics.linespace;
			}

			// Update previous UTF-32 character code
			previous_code = code;

			if (is_visible(code))
			{
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

				m_render_op.vertex_count += 6;

				// Update local-space bounds
				for (int i = 0; i < 4; ++i)
				{
					const math::fvec2& position = positions[i];
					for (int j = 0; j < 2; ++j)
					{
						m_local_bounds.min[j] = math::min(m_local_bounds.min[j], position[j]);
						m_local_bounds.max[j] = math::max(m_local_bounds.max[j], position[j]);
					}
				}
			}
		}

		// Upload vertex data to VBO, growing VBO size if necessary
		if (m_vertex_buffer->size() < m_render_op.vertex_count * text_vertex_stride)
		{
			m_vertex_buffer->resize(m_render_op.vertex_count * text_vertex_stride, {reinterpret_cast<const std::byte*>(m_vertex_data.data()), m_render_op.vertex_count * text_vertex_stride});
		}
		else
		{
			update_vertex_buffer();
		}

		// Update world-space bounds
		transformed();
	}

	void text::update_vertex_buffer()
	{
		m_vertex_buffer->write({reinterpret_cast<const std::byte*>(m_vertex_data.data()), m_render_op.vertex_count * text_vertex_stride});
	}

	bool text::is_visible(char32_t code) const
	{
		if (code == ' ' || code == '\t' || code == '\n' || code == '\r')
		{
			return false;
		}

		return true;
	}
}

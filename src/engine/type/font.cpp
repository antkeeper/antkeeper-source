// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/font.hpp>
#include <engine/debug/log.hpp>
#include <engine/type/unicode.hpp>
#include <bit>
#include <stdexcept>

namespace {
	
	/// Returns the next power of two for any unsigned integer.
	std::uint32_t next_power_of_two(std::uint32_t n)
	{
		return std::uint32_t{1} << std::bit_width(n);
	}
}

namespace type {

font::font(std::shared_ptr<typeface> face, float size, bool sdf):
	m_typeface(face),
	m_size(size),
	m_sdf(sdf)
{
	if (!m_typeface)
	{
		throw std::invalid_argument("Font has invalid typeface.");
	}
	
	// Get font metrics
	m_metrics = m_typeface->get_font_metrics(m_size);
	
	// Allocate font texture
	m_texture = std::make_shared<gl::texture_2d>
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r8_unorm,
				256,
				256
			)
		),
		std::make_shared<gl::sampler>
		(
			gl::sampler_filter::linear,
			gl::sampler_filter::linear,
			gl::sampler_mipmap_mode::linear,
			gl::sampler_address_mode::clamp_to_edge,
			gl::sampler_address_mode::clamp_to_edge
		)
	);
	
	// Init glyph pack
	const auto& texture_dimensions = m_texture->get_image_view()->get_image()->get_dimensions();
	m_glyph_pack.resize({texture_dimensions[0], texture_dimensions[1]});
}

std::size_t font::cache_glyph(char32_t code)
{
	return cache_glyphs(code, code);
}

std::size_t font::cache_glyphs(char32_t first, char32_t last)
{
	std::vector<glyph*> cached_glyphs;
	bool repack = false;
	
	// Load glyphs
	for (auto code = first; code <= last; ++code)
	{
		// Check if typeface contains glyph for the given character code
		if (!m_typeface->has_glyph(code))
		{
			// Glyph not found, use missing glyph (character code 0)
			code = 0;
		}
		
		// Ignore glyph if already cached
		if (auto it = m_glyph_map.find(code); it != m_glyph_map.end())
		{
			continue;
		}
		
		// Load glyph from typeface and insert into glyph map
		auto result = m_glyph_map.emplace(code, m_typeface->get_glyph(code, m_size, m_sdf));
		if (result.second)
		{
			// Append pointer to glyph to vector of newly-cached glyphs
			auto g = cached_glyphs.emplace_back(&result.first->second);
			
			// Attempt to pack glyph
			if (!repack)
			{
				if (auto node = m_glyph_pack.insert({g->bitmap_dimensions[0], g->bitmap_dimensions[1]}))
				{
					// Update texture coordinates of glyph
					g->bitmap_position[0] = node->bounds.min.x();
					g->bitmap_position[1] = node->bounds.min.y();
				}
				else
				{
					const auto& texture_dimensions = m_texture->get_image_view()->get_image()->get_dimensions();
					debug::log_trace
					(
						"Failed to pack glyph for U+{:X} into {}x{} texture",
						static_cast<std::uint32_t>(code),
						texture_dimensions[0],
						texture_dimensions[1]
					);
					
					// Glyph pack failed, repack later
					repack = true;
				}
			}
		}
	}
	
	if (!repack)
	{
		// Write glyph bitmaps to font texture
		write_bitmaps(cached_glyphs);
	}
	else
	{
		// Resize texture and repack all glyphs
		repack_glyphs();
	}
	
	return cached_glyphs.size();
}

std::size_t font::cache_glyphs(std::u32string_view text)
{
	std::vector<glyph*> cached_glyphs;
	bool repack = false;
	
	// Load glyphs
	for (auto code: text)
	{
		// Check if typeface contains glyph for the given character code
		if (!m_typeface->has_glyph(code))
		{
			// Glyph not found, use missing glyph (character code 0)
			code = 0;
		}
		
		// Ignore glyph if already cached
		if (auto it = m_glyph_map.find(code); it != m_glyph_map.end())
		{
			continue;
		}
		
		// Load glyph from typeface and insert into glyph map
		auto result = m_glyph_map.emplace(code, m_typeface->get_glyph(code, m_size, m_sdf));
		if (result.second)
		{
			// Append pointer to glyph to vector of newly-cached glyphs
			auto g = cached_glyphs.emplace_back(&result.first->second);
			
			// Attempt to pack glyph
			if (!repack)
			{
				if (auto node = m_glyph_pack.insert({g->bitmap_dimensions[0], g->bitmap_dimensions[1]}))
				{
					// Update texture coordinates of glyph
					g->bitmap_position[0] = node->bounds.min.x();
					g->bitmap_position[1] = node->bounds.min.y();
				}
				else
				{
					const auto& texture_dimensions = m_texture->get_image_view()->get_image()->get_dimensions();
					debug::log_trace
					(
						"Failed to pack glyph for U+{:X} into {}x{} texture",
						static_cast<std::uint32_t>(code),
						texture_dimensions[0],
						texture_dimensions[1]
					);
					
					// Glyph pack failed, repack later
					repack = true;
				}
			}
		}
	}
	
	if (!repack)
	{
		// Write glyph bitmaps to font texture
		write_bitmaps(cached_glyphs);
	}
	else
	{
		// Resize texture and repack all glyphs
		repack_glyphs();
	}
	
	return cached_glyphs.size();
}

std::size_t font::cache_glyphs(std::string_view text)
{
	auto utf32 = to_utf32(text);
	return cache_glyphs(utf32);
}

const glyph* font::get_cached_glyph(char32_t code) const
{
	// Check if typeface contains glyph for the given character code
	if (!m_typeface->has_glyph(code))
	{
		// Glpyh not found, use missing glyph (character code 0)
		code = 0;
	}
	
	if (auto it = m_glyph_map.find(code); it != m_glyph_map.end())
	{
		// Return pointer to cached glyph
		return &it->second;
	}
	
	// Glyph not cached
	return nullptr;
}

std::array<float, 2> font::get_kerning(char32_t first, char32_t second) const
{
	return m_typeface->get_kerning(m_size, first, second);
}

void font::write_bitmaps(std::span<glyph*> glyphs)
{
	for (auto g: glyphs)
	{
		m_texture->get_image_view()->get_image()->write
		(
			0,
			g->bitmap_position[0],
			g->bitmap_position[1],
			0,
			g->bitmap_dimensions[0],
			g->bitmap_dimensions[1],
			1,
			m_texture->get_image_view()->get_image()->get_format(),
			{g->bitmap_data.get(), g->bitmap_dimensions[0] * g->bitmap_dimensions[1]}
		);
	}
}

void font::repack_glyphs()
{
	// Get font texture dimensions
	const auto old_texture_dimensions = m_texture->get_image_view()->get_image()->get_dimensions();
	
	// Init new dimensions of font texture
	auto new_texture_dimensions = old_texture_dimensions;
	
	// Pack glyphs
	bool packed = false;
	do
	{
		// Determine new dimensions of font texture
		if (new_texture_dimensions[0] > new_texture_dimensions[1])
		{
			new_texture_dimensions[1] = next_power_of_two(new_texture_dimensions[1]);
		}
		else
		{
			new_texture_dimensions[0] = next_power_of_two(new_texture_dimensions[0]);
		}
		
		debug::log_trace
		(
			"Repacking {} glyphs into {}x{} texture...",
			m_glyph_map.size(),
			new_texture_dimensions[0],
			new_texture_dimensions[1]
		);
		
		// Resize glyph pack
		m_glyph_pack.resize({new_texture_dimensions[0], new_texture_dimensions[1]});
		
		// Attempt to pack glyphs
		packed = true;
		for (auto it = m_glyph_map.begin(); it != m_glyph_map.end(); ++it)
		{
			// Pack glyph
			if (auto pack_node = m_glyph_pack.insert({it->second.bitmap_dimensions[0], it->second.bitmap_dimensions[1]}))
			{
				// Update texture coordinates of glyph
				it->second.bitmap_position[0] = pack_node->bounds.min.x();
				it->second.bitmap_position[1] = pack_node->bounds.min.y();
			}
			else
			{
				debug::log_trace
				(
					"Failed to pack glyph for U+{:X} into {}x{} texture",
					static_cast<std::uint32_t>(it->first),
					new_texture_dimensions[0],
					new_texture_dimensions[1]
				);
				
				// Glyph pack failed
				packed = false;
				break;
			}
		}
	}
	while (!packed);
	
	debug::log_trace
	(
		"Packed {} glyphs into {}x{} texture",
		m_glyph_map.size(),
		new_texture_dimensions[0],
		new_texture_dimensions[1]
	);
	
	debug::log_trace
	(
		"Resizing font texture from {}x{} to {}x{}...",
		old_texture_dimensions[0],
		old_texture_dimensions[1],
		new_texture_dimensions[0],
		new_texture_dimensions[1]
	);
	
	// Resize font texture image
	m_texture->set_image_view
	(
		std::make_shared<gl::image_view_2d>
		(
			std::make_shared<gl::image_2d>
			(
				gl::format::r8_unorm,
				new_texture_dimensions[0],
				new_texture_dimensions[1]
			)
		)
	);
	
	debug::log_trace
	(
		"Resized font texture from {}x{} to {}x{}",
		old_texture_dimensions[0],
		old_texture_dimensions[1],
		new_texture_dimensions[0],
		new_texture_dimensions[1]
	);
	
	debug::log_trace
	(
		"Writing {} glyphs to font texture...",
		m_glyph_map.size()
	);
	
	// Write glyph bitmaps to font texture image
	for (auto it = m_glyph_map.begin(); it != m_glyph_map.end(); ++it)
	{
		m_texture->get_image_view()->get_image()->write
		(
			0,
			it->second.bitmap_position[0],
			it->second.bitmap_position[1],
			0,
			it->second.bitmap_dimensions[0],
			it->second.bitmap_dimensions[1],
			1,
			m_texture->get_image_view()->get_image()->get_format(),
			{it->second.bitmap_data.get(), it->second.bitmap_dimensions[0] * it->second.bitmap_dimensions[1]}
		);
	}
	
	debug::log_trace
	(
		"Wrote {} glyphs to font texture",
		m_glyph_map.size()
	);
	
	// Generate font texture resized event
	m_texture_resized_publisher.publish({this});
}

} // namespace type

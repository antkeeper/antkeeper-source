// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/type/bitmap-font.hpp>
#include <engine/geom/rect-pack.hpp>
#include <stdexcept>

namespace type {

bitmap_font::bitmap_font(const font_metrics& metrics):
	font(metrics)
{}

bitmap_font::bitmap_font():
	bitmap_font(font_metrics{})
{}

bool bitmap_font::contains(char32_t code) const
{
	return m_glyphs.count(code) != 0;
}

bitmap_glyph& bitmap_font::insert(char32_t code)
{
	return std::get<0>(m_glyphs.try_emplace(code))->second;
}

void bitmap_font::remove(char32_t code)
{
	if (auto it = m_glyphs.find(code); it != m_glyphs.end())
	{
		m_glyphs.erase(it);
	}
}

void bitmap_font::clear()
{
	m_glyphs.clear();
}

bool bitmap_font::pack(bool resize)
{
	// Returns the smallest power of two that is not smaller than @p x.
	auto ceil2 = [](std::uint32_t x) -> std::uint32_t
	{
		if (x <= 1)
			return 1;
		std::uint32_t y = 2;
		--x;
		while (x >>= 1)
			y <<= 1;
		return y;
	};
	
	// Calculate initial size of the font bitmap
	std::uint32_t bitmap_w = 0;
	std::uint32_t bitmap_h = 0;
	if (resize)
	{
		// Find the maximum glyph dimensions
		std::uint32_t max_glyph_w = 0;
		std::uint32_t max_glyph_h = 0;
		for (auto it = m_glyphs.begin(); it != m_glyphs.end(); ++it)
		{
			max_glyph_w = std::max(max_glyph_w, it->second.bitmap_width);
			max_glyph_h = std::max(max_glyph_h, it->second.bitmap_height);
		}
		
		// Find minimum power of two dimensions that can accommodate maximum glyph dimensions
		bitmap_w = ceil2(max_glyph_w);
		bitmap_h = ceil2(max_glyph_h);
	}
	else if (m_texture)
	{
		bitmap_w = m_texture->get_image_view()->get_image()->get_dimensions()[0];
		bitmap_h = m_texture->get_image_view()->get_image()->get_dimensions()[1];
	}
	
	bool packed = false;
	geom::rect_pack<std::uint32_t> glyph_pack(bitmap_w, bitmap_h);
	std::unordered_map<char32_t, const typename geom::rect_pack<std::uint32_t>::node_type*> glyph_map;
	
	while (!packed)
	{
		// For each glyph
		for (auto it = m_glyphs.begin(); it != m_glyphs.end(); ++it)
		{
			// Attempt to pack glyph bitmap
			const auto* node = glyph_pack.pack(it->second.bitmap_width, it->second.bitmap_height);
			
			// Abort if packing failed
			if (!node)
				break;
			
			// Map pack node to glyph character code
			glyph_map[it->first] = node;
		}
		
		// Check if not all glyphs were packed
		if (glyph_map.size() != m_glyphs.size())
		{
			if (!resize)
			{
				// No resize, packing failed
				packed = false;
				break;
			}
			
			// Clear glyph map
			glyph_map.clear();
			
			// Clear glyph pack
			glyph_pack.clear();
			
			// Resize glyph pack
			if (bitmap_w > bitmap_h)
				bitmap_h = ceil2(++bitmap_h);
			else
				bitmap_w = ceil2(++bitmap_w);
			glyph_pack.resize(bitmap_w, bitmap_h);
		}
		else
		{
			packed = true;
		}
	}
	
	// Copy glyph bitmaps into font bitmap
	if (packed)
	{
		// Resize bitmap font image
		if (!m_texture ||
			bitmap_w != m_texture->get_image_view()->get_image()->get_dimensions()[0] ||
			bitmap_h != m_texture->get_image_view()->get_image()->get_dimensions()[1])
		{
			
			// Construct font bitmap sampler
			if (!m_sampler)
			{
				m_sampler = std::make_shared<gl::sampler>
				(
					gl::sampler_filter::linear,
					gl::sampler_filter::linear,
					gl::sampler_mipmap_mode::linear,
					gl::sampler_address_mode::clamp_to_edge,
					gl::sampler_address_mode::clamp_to_edge
				);
			}
			
			const std::uint32_t mip_count = static_cast<std::uint32_t>(std::bit_width(std::max(bitmap_w, bitmap_h)));
			m_texture = std::make_shared<gl::texture_2d>
			(
				std::make_shared<gl::image_view_2d>
				(
					std::make_shared<gl::image_2d>
					(
						gl::format::r8_unorm,
						bitmap_w,
						bitmap_h,
						mip_count
					),
					gl::format::r8_unorm,
					0,
					mip_count
				),
				m_sampler
			);
		}
		
		// For each glyph
		for (auto it = m_glyphs.begin(); it != m_glyphs.end(); ++it)
		{
			// Find rect pack node corresponding to the glyph
			const auto* node = glyph_map[it->first];
			
			// Write glyph bitmap data into bitmap font image
			m_texture->get_image_view()->get_image()->write
			(
				0,
				node->bounds.min.x(),
				node->bounds.min.y(),
				0,
				it->second.bitmap_width,
				it->second.bitmap_height,
				1,
				gl::format::r8_unorm,
				it->second.bitmap
			);
			
			// Record coordinates of glyph bitmap within font bitmap
			it->second.position = {node->bounds.min.x(), node->bounds.min.y()};
		}
		
		// Regenerate mipmaps
		m_texture->get_image_view()->get_image()->generate_mipmaps();
	}
	
	return packed;
}

const glyph_metrics& bitmap_font::get_glyph_metrics(char32_t code) const
{
	if (auto it = m_glyphs.find(code); it != m_glyphs.end())
	{
		return it->second.metrics;
	}
	throw std::invalid_argument("Cannot fetch metrics of unknown bitmap glyph");
}

const bitmap_glyph* bitmap_font::get_glyph(char32_t code) const
{
	if (auto it = m_glyphs.find(code); it != m_glyphs.end())
	{
		return &it->second;
	}
	
	return nullptr;
}

bitmap_glyph* bitmap_font::get_glyph(char32_t code)
{
	if (auto it = m_glyphs.find(code); it != m_glyphs.end())
	{
		return &it->second;
	}
	
	return nullptr;
}

} // namespace type

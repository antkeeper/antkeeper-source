// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_MATERIAL_BLEND_MODE_HPP
#define ANTKEEPER_RENDER_MATERIAL_BLEND_MODE_HPP

#include <cstdint>

namespace render {

/// Material blend modes.
enum class material_blend_mode: std::uint8_t
{
	/// Material is fully opaque.
	opaque,
	
	/// Material has binary masked opacity.
	masked,
	
	/// Material is translucent.
	translucent
};

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_BLEND_MODE_HPP

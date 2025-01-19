// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_RENDER_MATERIAL_SHADOW_MODE_HPP
#define ANTKEEPER_RENDER_MATERIAL_SHADOW_MODE_HPP

#include <cstdint>

namespace render {

/// Material shadow casting modes.
enum class material_shadow_mode: std::uint8_t
{
	/// Material does not cast shadows.
	none,
	
	/// Material casts fully opaque shadows.
	opaque
};

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_SHADOW_MODE_HPP

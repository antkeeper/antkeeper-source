// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_LIGHT_TYPE_HPP
#define ANTKEEPER_SCENE_LIGHT_TYPE_HPP

#include <cstdint>

namespace scene {

/// Light types.
enum class light_type: std::uint8_t
{
	/// Directional light.
	directional,
	
	/// Spot light.
	spot,
	
	/// Point light.
	point,
	
	/// Rectangle light.
	rectangle
};

} // namespace scene

#endif // ANTKEEPER_SCENE_LIGHT_TYPE_HPP

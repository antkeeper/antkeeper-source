// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_LIGHT_TYPE_HPP
#define ANTKEEPER_SCENE_LIGHT_TYPE_HPP

namespace scene {

/// Light types.
enum class light_type
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

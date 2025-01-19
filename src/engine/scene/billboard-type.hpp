// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_SCENE_BILLBOARD_TYPE_HPP
#define ANTKEEPER_SCENE_BILLBOARD_TYPE_HPP

#include <cstdint>

namespace scene {

/// Billboard types.
enum class billboard_type: std::uint8_t
{
	/// Billboard is unaligned.
	flat,
	
	/// Billboard aligns to face camera.
	spherical,
	
	/// Billboard rotates about an alignment axis to face camera.
	cylindrical
};

} // namespace scene

#endif // ANTKEEPER_SCENE_BILLBOARD_TYPE_HPP

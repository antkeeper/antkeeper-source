// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_DISPLAY_ORIENTATION_HPP
#define ANTKEEPER_APP_DISPLAY_ORIENTATION_HPP

#include <cstdint>

namespace app {

/**
 * Display orientations.
 */
enum class display_orientation: std::uint8_t
{
	/// Display orientation unknown.
	unknown,
	
	/// Display is in landscape mode, with the right side up, relative to portrait mode.
	landscape,
	
	/// Display is in landscape mode, with the left side up, relative to portrait mode.
	landscape_flipped,
	
	/// Display is in portrait mode.
	portrait,
	
	/// Display is in portrait mode, upside down.
	portrait_flipped
};

} // namespace app

#endif // ANTKEEPER_APP_DISPLAY_ORIENTATION_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.app.display_orientation;

export namespace engine::app
{
	/// Display orientations.
	enum class display_orientation
	{
		/// Unknown display orientation.
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
}

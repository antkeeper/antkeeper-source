// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.app.display_events;
export import engine.app.display_orientation;

export namespace engine::app
{
	class display;

	/// Event generated when a display has been connected.
	struct display_connected_event
	{
		/// Pointer to the display that has been connected.
		const display* display{nullptr};
	};

	/// Event generated when a display has been disconnected.
	struct display_disconnected_event
	{
		/// Pointer to the display that has been disconnected.
		const display* display{nullptr};
	};

	/// Event generated when the orientation of a display has changed.
	struct display_orientation_changed_event
	{
		/// Pointer to the display that has had it's orientation changed.
		const display* display{nullptr};
	
		/// Orientation of the display.
		display_orientation orientation{0};
	};
}

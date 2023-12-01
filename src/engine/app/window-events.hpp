// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_APP_WINDOW_EVENTS_HPP
#define ANTKEEPER_APP_WINDOW_EVENTS_HPP

#include <engine/math/vector.hpp>

namespace app {

class window;

/**
 * Event generated when a window has been requested to close.
 */
struct window_closed_event
{
	/// Pointer to the window that has been requested to close.
	window* window{nullptr};
};

/**
 * Event generated when a window has gained or lost focus.
 */
struct window_focus_changed_event
{
	/// Pointer to the window that has gained or lost focus.
	window* window{nullptr};
	
	/// `true` if the window is in focus, `false` otherwise.
	bool in_focus{false};
};

/**
 * Event generated when a window has been moved.
 */
struct window_moved_event
{
	/// Pointer to the window that has been moved.
	window* window{nullptr};
	
	/// Position of the window, in display units.
	math::ivec2 position{0, 0};
};

/**
 * Event generated when a window has been maximized.
 */
struct window_maximized_event
{
	/// Pointer to the window that has been maximized.
	window* window{nullptr};
};

/**
 * Event generated when a window has been minimized.
 */
struct window_minimized_event
{
	/// Pointer to the window that has been minimized.
	window* window{nullptr};
};

/**
 * Event generated when a window has been restored.
 */
struct window_restored_event
{
	/// Pointer to the window that has been restored.
	window* window;
};

/**
 * Event generated when a window has been resized.
 */
struct window_resized_event
{
	/// Pointer to the window that has been resized.
	window* window{nullptr};
	
	/// Window size, in display units.
	math::ivec2 size{0, 0};
};

} // namespace app

#endif // ANTKEEPER_APP_WINDOW_EVENTS_HPP

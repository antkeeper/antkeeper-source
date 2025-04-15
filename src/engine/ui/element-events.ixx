// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.ui.element_events;
export import engine.math.vector;
import engine.input.mouse;

export namespace engine::ui
{
	class element;

	/// Event generated when an element has gained or lost focus.
	struct element_focus_changed_event
	{
		/// Pointer to the element that has gained or lost focus.
		element* element{nullptr};

		/// `true` if the element has focus, `false` otherwise.
		bool has_focus{false};
	};

	/// Event generated when a mouse moves within the bounds of an element.
	struct element_mouse_moved_event
	{
		/// Pointer to the element in which the mouse has moved.
		element* element{nullptr};

		/// Pointer to the mouse.
		input::mouse* mouse{nullptr};

		/// Mouse position, in pixels, relative to the window.
		math::fvec2 position{};

		/// Relative movement of the mouse, in pixels.
		math::fvec2 difference{};
	};

	/// Event generated when a mouse enters the bounds of an element.
	struct element_mouse_entered_event
	{
		/// Pointer to the element that the mouse has entered.
		element* element{nullptr};

		/// Pointer to the mouse.
		input::mouse* mouse{nullptr};

		/// Position of the mouse when the mouse entered.
		math::fvec2 position{};
	};

	/// Event generated when a mouse exits the bounds of an element.
	struct element_mouse_exited_event
	{
		/// Pointer to the element that the mouse has exited.
		element* element{nullptr};

		/// Pointer to the mouse.
		input::mouse* mouse{nullptr};

		/// Position of the mouse when the mouse exited.
		math::fvec2 position{};
	};

	/// Event generated when a mouse button is pressed on an element.
	struct element_mouse_button_pressed_event
	{
		/// Pointer to the element on which a mouse button has been pressed.
		element* element{nullptr};

		/// Pointer to the mouse.
		input::mouse* mouse{nullptr};

		/// Position of the mouse when the button was pressed.
		math::fvec2 position{};

		/// Mouse button being pressed.
		input::mouse_button button{};
	};

	/// Event generated when a mouse button is released on an element.
	struct element_mouse_button_released_event
	{
		/// Pointer to the element on which a mouse button has been released.
		element* element{nullptr};

		/// Pointer to the mouse.
		input::mouse* mouse{nullptr};

		/// Position of the mouse when the button was released.
		math::fvec2 position{};

		/// Mouse button being released.
		input::mouse_button button{};
	};
}

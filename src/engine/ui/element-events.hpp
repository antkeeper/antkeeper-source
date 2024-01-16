// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UI_ELEMENT_EVENTS_HPP
#define ANTKEEPER_UI_ELEMENT_EVENTS_HPP

#include <engine/input/mouse.hpp>

namespace ui {

class element;

/**
 * Event generated when an element has gained or lost focus.
 */
struct element_focus_changed_event
{
	/** Pointer to the element that has gained or lost focus. */
	element* element{nullptr};
	
	/** `true` if the element has focus, `false` otherwise. */
	bool has_focus{false};
};

/**
 * Event generated when a mouse enters the bounds of an element.
 */
struct element_mouse_entered_event
{
	/** Pointer to the element that the mouse has entered. */
	element* element{nullptr};
	
	/** Pointer to the mouse. */
	input::mouse* mouse{nullptr};
	
	/** Position of the mouse when the mouse entered. */
	math::ivec2 position{};
};

/**
 * Event generated when a mouse exits the bounds of an element.
 */
struct element_mouse_exited_event
{
	/** Pointer to the element that the mouse has exited. */
	element* element{nullptr};
	
	/** Pointer to the mouse. */
	input::mouse* mouse{nullptr};
	
	/** Position of the mouse when the mouse exited. */
	math::ivec2 position{};
};

/**
 * Event generated when a mouse button is pressed on an element.
 */
struct element_mouse_button_pressed_event
{
	/** Pointer to the element on which a mouse button has been pressed. */
	element* element{nullptr};
	
	/** Pointer to the mouse. */
	input::mouse* mouse{nullptr};
	
	/** Position of the mouse when the button was pressed. */
	math::ivec2 position{};
	
	/** Mouse button being pressed. */
	input::mouse_button button{};
};

/**
 * Event generated when a mouse button is released on an element.
 */
struct element_mouse_button_released_event
{
	/** Pointer to the element on which a mouse button has been released. */
	element* element{nullptr};
	
	/** Pointer to the mouse. */
	input::mouse* mouse{nullptr};
	
	/** Position of the mouse when the button was released. */
	math::ivec2 position{};
	
	/** Mouse button being released. */
	input::mouse_button button{};
};

} // namespace ui

#endif // ANTKEEPER_UI_ELEMENT_EVENTS_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MAPPING_HPP
#define ANTKEEPER_INPUT_MAPPING_HPP

#include <engine/input/mapping-type.hpp>
#include <engine/input/gamepad-axis.hpp>
#include <engine/input/gamepad-button.hpp>
#include <engine/input/mouse-button.hpp>
#include <engine/input/mouse-motion-axis.hpp>
#include <engine/input/mouse-scroll-axis.hpp>
#include <engine/input/scancode.hpp>
#include <cstdint>

namespace input {

class control;
class gamepad;
class keyboard;
class mouse;

/**
 * Abstract base class for input mappings.
 */
class mapping
{
public:
	/**
	 * Constructs an input mapping.
	 */
	mapping() = default;
	
	/// Destructs an input mapping.
	virtual ~mapping() = default;
	
	/// Returns the input mapping type.
	[[nodiscard]] virtual constexpr mapping_type get_mapping_type() const noexcept = 0;
};

/**
 * Maps a direction along a gamepad axis to a control input value.
 */
class gamepad_axis_mapping: public mapping
{
public:
	/**
	 * Constructs a gamepad axis mapping.
	 *
	 * @param gamepad Pointer to the gamepad to map, or `nullptr` if input from any gamepad will be mapped.
	 * @param axis Gamepad axis to map.
	 * @param direction Sign bit of the direction to map.
	 */
	/// @{
	gamepad_axis_mapping(input::gamepad* gamepad, gamepad_axis axis, bool direction);
	gamepad_axis_mapping() = default;
	/// @}
	
	/// Returns mapping_type::gamepad_axis.
	[[nodiscard]] inline constexpr mapping_type get_mapping_type() const noexcept override
	{
		return mapping_type::gamepad_axis;
	}
	
	/// Pointer to the mapped gamepad, or `nullptr` if input from any gamepad is accepted.
	input::gamepad* gamepad{nullptr};
	
	/// Mapped gamepad axis.
	gamepad_axis axis{0};
	
	/// Sign bit of the mapped direction.
	bool direction{false};
};

/**
 * Maps a gamepad button to a control input value.
 */
class gamepad_button_mapping: public mapping
{
public:
	/**
	 * Constructs a gamepad button mapping.
	 *
	 * @param gamepad Pointer to the gamepad to map, or `nullptr` if input from any gamepad will be mapped.
	 * @param button Gamepad button to map.
	 */
	/// @{
	gamepad_button_mapping(input::gamepad* gamepad, gamepad_button button);
	gamepad_button_mapping() = default;
	/// @}
	
	/// Returns mapping_type::gamepad_button.
	[[nodiscard]] inline constexpr mapping_type get_mapping_type() const noexcept override
	{
		return mapping_type::gamepad_button;
	}
	
	/// Pointer to the mapped gamepad, or `nullptr` if input from any gamepad is accepted.
	input::gamepad* gamepad{nullptr};
	
	/// Mapped gamepad button.
	gamepad_button button{0};
};

/**
 * Maps a keyboard key to a control input value.
 */
class key_mapping: public mapping
{
public:
	/**
	 * Constructs a key mapping.
	 *
	 * @param keyboard Pointer to the keyboard to map, or `nullptr` if input from any keyboard will be mapped.
	 * @param scancode Scancode of the key to map.
	 * @param repeat `false` if the mapping should ignore key repeats, `true` otherwise.
	 * @param modifiers Modifier keys bitmask.
	 */
	/// @{
	key_mapping(input::keyboard* keyboard, input::scancode scancode, std::uint16_t modifiers = 0, bool repeat = false);
	key_mapping() = default;
	/// @}
	
	/// Returns mapping_type::key.
	[[nodiscard]] inline constexpr mapping_type get_mapping_type() const noexcept override
	{
		return mapping_type::key;
	}
	
	/// Pointer to the mapped keyboard, or `nullptr` if input from any keyboard is accepted.
	input::keyboard* keyboard{nullptr};
	
	/// Scancode of the mapped key.
	scancode scancode{0};
	
	/// Modifier keys bitbask.
	std::uint16_t modifiers{0};
	
	/// `false` if the mapping ignores key repeats, `true` otherwise.
	bool repeat{false};
};

/**
 * Maps a mouse button to a control input value.
 */
class mouse_button_mapping: public mapping
{
public:
	/**
	 * Constructs a mouse button mapping.
	 *
	 * @param mouse Pointer to the mouse to map, or `nullptr` if input from any mouse will be mapped.
	 * @param button Mouse button to map.
	 */
	/// @{
	mouse_button_mapping(input::mouse* mouse, mouse_button button);
	mouse_button_mapping() = default;
	/// @}
	
	/// Returns mapping_type::mouse_button.
	[[nodiscard]] inline constexpr mapping_type get_mapping_type() const noexcept override
	{
		return mapping_type::mouse_button;
	}
	
	/// Pointer to the mapped mouse, or `nullptr` if input from any mouse is accepted.
	input::mouse* mouse{nullptr};
	
	/// Mapped mouse button.
	mouse_button button{0};
};

/**
 * Maps a direction along a mouse motion axis to a control input value.
 */
class mouse_motion_mapping: public mapping
{
public:
	/**
	 * Constructs a mouse motion mapping.
	 *
	 * @param mouse Pointer to the mouse to map, or `nullptr` if input from any mouse will be mapped.
	 * @param axis Mouse motion axis to map.
	 * @param direction Sign bit of the direction to map.
	 */
	/// @{
	mouse_motion_mapping(input::mouse* mouse, mouse_motion_axis axis, bool direction);
	mouse_motion_mapping() = default;
	/// @}
	
	/// Returns mapping_type::mouse_motion.
	[[nodiscard]] inline constexpr mapping_type get_mapping_type() const noexcept override
	{
		return mapping_type::mouse_motion;
	}
	
	/// Pointer to the mapped mouse, or `nullptr` if input from any mouse is accepted.
	input::mouse* mouse{nullptr};
	
	/// Mapped mouse motion axis.
	mouse_motion_axis axis{0};
	
	/// Sign bit of the mapped direction.
	bool direction{false};
};

/**
 * Maps a direction along a mouse scroll axis to a control input value.
 */
class mouse_scroll_mapping: public mapping
{
public:
	/**
	 * Constructs a mouse scroll mapping.
	 *
	 * @param control Control to which input will be mapped.
	 * @param mouse Pointer to the mouse to map, or `nullptr` if input from any mouse will be mapped.
	 * @param axis Mouse scroll axis to map.
	 * @param direction Sign bit of the direction to map.
	 */
	/// @{
	mouse_scroll_mapping(input::mouse* mouse, mouse_scroll_axis axis, bool direction);
	mouse_scroll_mapping() = default;
	/// @}
	
	/// Returns mapping_type::mouse_scroll.
	[[nodiscard]] inline constexpr mapping_type get_mapping_type() const noexcept override
	{
		return mapping_type::mouse_scroll;
	}
	
	/// Pointer to the mapped mouse, or `nullptr` if input from any mouse is accepted.
	input::mouse* mouse{nullptr};
	
	/// Mapped mouse scroll axis.
	mouse_scroll_axis axis{0};
	
	/// Sign bit of the mapped direction.
	bool direction{false};
};

} // namespace input

#endif // ANTKEEPER_INPUT_MAPPING_HPP

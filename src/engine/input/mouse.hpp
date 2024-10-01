// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_MOUSE_HPP
#define ANTKEEPER_INPUT_MOUSE_HPP

#include <engine/input/device.hpp>
#include <engine/input/mouse-events.hpp>
#include <engine/input/mouse-button.hpp>
#include <engine/event/publisher.hpp>
#include <engine/math/vector.hpp>
#include <cstdint>

namespace input {

/// A virtual mouse which generates mouse-related input events.
class mouse: public device
{
public:
	/// Destructs a mouse.
	~mouse() override = default;
	
	/// Simulates a mouse button press.
	/// @param button Button to press.
	void press(mouse_button button);
	
	/// Simulates a mouse button release.	
	/// @param button Button to release.
	void release(mouse_button button);
	
	/// Simulates mouse movement.
	/// @param position Mouse position, in pixels, relative to the window.
	/// @param difference Relative movement of the mouse, in pixels.
	void move(const math::vec2<std::int32_t>& position, const math::vec2<std::int32_t>& difference);
	
	/// Simulates mouse scrolling.
	/// @param velocity Scroll velocity.
	void scroll(const math::fvec2& velocity);
	
	/// Returns the current mouse position, in pixels, relative to the window.
	[[nodiscard]] inline const math::vec2<std::int32_t>& get_position() const noexcept
	{
		return m_position;
	}
	
	/// Returns the channel through which mouse button pressed events are published.
	[[nodiscard]] inline ::event::channel<mouse_button_pressed_event>& get_button_pressed_channel() noexcept
	{
		return m_button_pressed_publisher.channel();
	}
	
	/// Returns the channel through which mouse button released events are published.
	[[nodiscard]] inline ::event::channel<mouse_button_released_event>& get_button_released_channel() noexcept
	{
		return m_button_released_publisher.channel();
	}
	
	/// Returns the channel through which mouse moved events are published.
	[[nodiscard]] inline ::event::channel<mouse_moved_event>& get_moved_channel() noexcept
	{
		return m_moved_publisher.channel();
	}
	
	/// Returns the channel through which mouse scrolled events are published.
	[[nodiscard]] inline ::event::channel<mouse_scrolled_event>& get_scrolled_channel() noexcept
	{
		return m_scrolled_publisher.channel();
	}
	
	/// Returns device_type::mouse.
	[[nodiscard]] inline constexpr device_type get_device_type() const noexcept override
	{
		return device_type::mouse;
	}
	
private:
	math::vec2<std::int32_t> m_position{0, 0};
	
	::event::publisher<mouse_button_pressed_event> m_button_pressed_publisher;
	::event::publisher<mouse_button_released_event> m_button_released_publisher;
	::event::publisher<mouse_moved_event> m_moved_publisher;
	::event::publisher<mouse_scrolled_event> m_scrolled_publisher;
};

} // namespace input

#endif // ANTKEEPER_INPUT_MOUSE_HPP

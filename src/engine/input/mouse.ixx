// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.input.mouse;
export import engine.input.device;
export import engine.event.channel;
export import engine.math.vector;
import engine.event.publisher;

export namespace engine::input
{
	class mouse;

	/// Mouse buttons.
	enum class mouse_button
	{
		/// Left mouse button.
		left = 1,

		/// Middle mouse button.
		middle = 2,

		/// Right mouse button.
		right = 3
	};

	/// Mouse scroll axes.
	enum class mouse_scroll_axis
	{
		/// X-axis.
		x = 0,

		/// Y-axis.
		y = 1
	};

	/// Mouse motion axes.
	enum class mouse_motion_axis
	{
		/// X-axis.
		x = 0,

		/// Y-axis.
		y = 1
	};

	/// Event generated when a mouse has been moved.
	struct mouse_moved_event
	{
		/// Mouse that generated the event.
		mouse* mouse{nullptr};

		/// Mouse position, in pixels, relative to the window.
		math::fvec2 position{0, 0};

		/// Relative movement of the mouse, in pixels.
		math::fvec2 difference{0, 0};
	};

	/// Event generated when a mouse button has been pressed.
	struct mouse_button_pressed_event
	{
		/// Mouse that generated the event.
		mouse* mouse{nullptr};

		/// Mouse position, in pixels, relative to the window, when the button was pressed.
		math::fvec2 position{0, 0};

		/// Mouse button being pressed.
		mouse_button button{0};
	};

	/// Event generated when a mouse button has been released.
	struct mouse_button_released_event
	{
		/// Mouse that generated the event.
		mouse* mouse{nullptr};

		/// Mouse position, in pixels, relative to the window, when the button was released.
		math::fvec2 position{0, 0};

		/// Mouse button being released.
		mouse_button button{0};
	};

	/// Event generated when a mouse has been scrolled.
	struct mouse_scrolled_event
	{
		/// Mouse that generated the event.
		mouse* mouse{nullptr};

		/// Mouse position, in pixels, relative to the window, when the mouse was scrolled.
		math::fvec2 position{0, 0};

		/// Scroll velocity.
		math::fvec2 velocity{0.0f, 0.0f};
	};

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
		void move(const math::fvec2& position, const math::fvec2& difference);
	
		/// Simulates mouse scrolling.
		/// @param velocity Scroll velocity.
		void scroll(const math::fvec2& velocity);
	
		/// Returns the current mouse position, in pixels, relative to the window.
		[[nodiscard]] inline const math::fvec2& get_position() const noexcept
		{
			return m_position;
		}
	
		/// Returns the channel through which mouse button pressed events are published.
		[[nodiscard]] inline auto& get_button_pressed_channel() noexcept
		{
			return m_button_pressed_publisher.channel();
		}
	
		/// Returns the channel through which mouse button released events are published.
		[[nodiscard]] inline auto& get_button_released_channel() noexcept
		{
			return m_button_released_publisher.channel();
		}
	
		/// Returns the channel through which mouse moved events are published.
		[[nodiscard]] inline auto& get_moved_channel() noexcept
		{
			return m_moved_publisher.channel();
		}
	
		/// Returns the channel through which mouse scrolled events are published.
		[[nodiscard]] inline auto& get_scrolled_channel() noexcept
		{
			return m_scrolled_publisher.channel();
		}
	
		/// Returns device_type::mouse.
		[[nodiscard]] inline constexpr device_type get_device_type() const noexcept override
		{
			return device_type::mouse;
		}
	
	private:
		math::fvec2 m_position{0, 0};
	
		event::publisher<mouse_button_pressed_event> m_button_pressed_publisher;
		event::publisher<mouse_button_released_event> m_button_released_publisher;
		event::publisher<mouse_moved_event> m_moved_publisher;
		event::publisher<mouse_scrolled_event> m_scrolled_publisher;
	};
}

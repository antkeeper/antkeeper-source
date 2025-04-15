// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.input.gamepad;
export import engine.input.device;
export import engine.event.channel;
import engine.event.publisher;

export namespace engine::input
{
	class gamepad;

	/// Gamepad axes.
	enum class gamepad_axis
	{
		/// Left stick X-axis.
		left_stick_x,

		/// Left stick Y-axis.
		left_stick_y,

		/// Right stick X-axis.
		right_stick_x,

		/// Right stick Y-axis.
		right_stick_y,

		/// Left trigger.
		left_trigger,

		/// Right trigger.
		right_trigger
	};

	/// Gamepad buttons.
	enum class gamepad_button
	{
		/// A button.
		a,

		/// B button.
		b,

		/// X button.
		x,

		/// Y button.
		y,

		/// Back button.
		back,

		/// Guide button.
		guide,

		/// Start button.
		start,

		/// Left stick button.
		left_stick,

		/// Right stick button.
		right_stick,

		/// Left shoulder button.
		left_shoulder,

		/// Right shoulder button.
		right_shoulder,

		/// D-pad up button.
		dpad_up,

		/// D-pad down button.
		dpad_down,

		/// D-pad left button.
		dpad_left,

		/// D-pad right button.
		dpad_right
	};

	/// Gamepad axis activation response curves.
	enum class gamepad_response_curve
	{
		/// Linear response curve.
		linear,
	
		/// Squared response curve.
		square,
	
		/// Cubed response curve.
		cube
	};

	/// Event generated when a gamepad button has been pressed.
	struct gamepad_button_pressed_event
	{
		/// Gamepad that generated the event.
		gamepad* gamepad{nullptr};

		/// Gamepad button being pressed.
		gamepad_button button{0};
	};

	/// Event generated when a gamepad button has been released.
	struct gamepad_button_released_event
	{
		/// Gamepad that generated the event.
		gamepad* gamepad{nullptr};

		/// Gamepad button being released.
		gamepad_button button{0};
	};

	/// Event generated when a gamepad axis has been moved.
	struct gamepad_axis_moved_event
	{
		/// Gamepad that generated the event.
		gamepad* gamepad{nullptr};

		/// Gamepad axis being moved.
		gamepad_axis axis{0};

		/// Position of the gamepad axis, on `[-1, 1]`.
		float position{0.0f};
	};

	/// A virtual gamepad which generates gamepad-related input events.
	class gamepad: public device
	{
	public:
		/// Constructs a gamepad input device.
		gamepad();
	
		/// Destructs a gamepad.
		~gamepad() override = default;
	
		/// Sets the activation threshold for a gamepad axis.
		/// @param axis Gamepad axis.
		/// @param min Axis minimum activation threshold.
		/// @param max Axis maximum activation threshold.
		void set_activation_threshold(gamepad_axis axis, float min, float max);
	
		/// Sets the activation response curve of an axis.
		/// @param axis Gamepad axis.
		/// @param curve Activation response curve.
		void set_response_curve(gamepad_axis axis, gamepad_response_curve curve);
	
		/// Sets the type of deadzone shape for the axes on the left stick.
		/// @param cross If `true`, the x and y axes are independently activated, if `false`, activation of the x and y axes are dependent on their combined magnitude.
		void set_left_deadzone_cross(bool cross);
	
		/// Sets the type of deadzone shape for the axes on the right stick.
		/// @param cross If `true`, the x and y axes are independently activated, if `false`, activation of the x and y axes are dependent on their combined magnitude.
		void set_right_deadzone_cross(bool cross);
	
		/// Sets the roundness of the deadzone for the axes on the left stick.
		/// @param roundness Roundness of the deadzone shape for non-cross deadzones. A value of `0.0` results in a square deadzone, while a value of `1.0` results in a circular deadzone. Values between `0.0` and `1.0` result in a rounded rectangle deadzone.
		void set_left_deadzone_roundness(float roundness);
	
		/// Sets the roundness of the deadzone for the axes on the right stick.
		/// @param roundness Roundness of the deadzone shape for non-cross deadzones. A value of `0.0` results in a square deadzone, while a value of `1.0` results in a circular deadzone. Values between `0.0` and `1.0` result in a rounded rectangle deadzone.
		void set_right_deadzone_roundness(float roundness);
	
		/// Simulates a gamepad button press.
		/// @param button Button to press.
		void press(gamepad_button button);
	
		/// Simulates a gamepad button release.
		/// @param button Button to release.
		void release(gamepad_button button);
	
		/// Simulates a gamepad axis movement.
		/// @param axis Gamepad axis.
		/// @param position Position on the axis, on `[-1, 1]`.
		void move(gamepad_axis axis, float position);
	
		/// Returns the channel through which gamepad button pressed events are published.
		[[nodiscard]] inline auto& get_button_pressed_channel() noexcept
		{
			return button_pressed_publisher.channel();
		}
	
		/// Returns the channel through which gamepad button released events are published.
		[[nodiscard]] inline auto& get_button_released_channel() noexcept
		{
			return button_released_publisher.channel();
		}
	
		/// Returns the channel through which gamepad axis moved events are published.
		[[nodiscard]] inline auto& get_axis_moved_channel() noexcept
		{
			return axis_moved_publisher.channel();
		}
	
		/// Returns device_type::gamepad.
		[[nodiscard]] inline constexpr device_type get_device_type() const noexcept override
		{
			return device_type::gamepad;
		}
	
	private:
		void handle_axial_motion(gamepad_axis axis);
		void handle_biaxial_motion(gamepad_axis axis_x, gamepad_axis axis_y);
		float curve_response(gamepad_axis axis, float response) const;
	
		float axis_positions[6];
		float axis_activation_min[6];
		float axis_activation_max[6];
		gamepad_response_curve axis_response_curves[6];
		bool left_deadzone_cross{true};
		bool right_deadzone_cross{true};
		float left_deadzone_roundness{0.0f};
		float right_deadzone_roundness{0.0f};
	
		event::publisher<gamepad_button_pressed_event> button_pressed_publisher;
		event::publisher<gamepad_button_released_event> button_released_publisher;
		event::publisher<gamepad_axis_moved_event> axis_moved_publisher;
	};
}

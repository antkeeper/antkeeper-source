// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.input.action;
export import <functional>;
export import engine.event.channel;
import engine.event.publisher;

export namespace engine::input
{
	class action;

	/// Event generated when an action has been activated.
	struct action_activated_event
	{
		/// Control that was activated.
		action* action{nullptr};
	};

	/// Event generated while an action is active.
	struct action_active_event
	{
		/// Active action.
		action* action{nullptr};

		/// Control input value.
		float input_value{0.0f};
	};

	/// Event generated when an action has been deactivated.
	struct action_deactivated_event
	{
		/// Control that was deactivated.
		action* action{nullptr};
	};

	/// Evaluates an activation state given input values and publishes events on activation state changes.
	class action
	{
	public:
		/// Threshold function type.
		/// @details Given an input value, returns `true` if the action should be considered active, and `false` otherwise.
		using threshold_function_type = std::function<bool(float)>;
	
		/// Constructs an action.
		action();
	
		/// Sets the threshold function.
		/// @param function Threshold function.
		inline void set_threshold_function(const threshold_function_type& function) noexcept
		{
			m_threshold_function = function;
		}
	
		/// Evaluates the activation state of the action, according to its threshold function and an input value.
		/// @param value Input value.
		void evaluate(float value);
	
		/// Publishes an action active event if the action is active.
		void update() const;
	
		/// Resets the activation state of the action without publishing any events.
		void reset() noexcept;
	
		/// Returns the threshold function.
		[[nodiscard]] inline const threshold_function_type& get_threshold_function() const noexcept
		{
			return m_threshold_function;
		}
	
		/// Returns `true` if the action is active, `false` otherwise.
		[[nodiscard]] inline bool is_active() const noexcept
		{
			return m_active;
		}
	
		/// Returns the most recently evaluated input value.
		[[nodiscard]] inline float get_input_value() const noexcept
		{
			return m_active_event.input_value;
		}
	
		/// Returns the channel through which action activated events are published.
		[[nodiscard]] inline auto& get_activated_channel() noexcept
		{
			return m_activated_publisher.channel();
		}
	
		/// Returns the channel through which action active events are published.
		[[nodiscard]] inline auto& get_active_channel() noexcept
		{
			return m_active_publisher.channel();
		}
	
		/// Returns the channel through which action deactivated events are published.
		[[nodiscard]] inline auto& get_deactivated_channel() noexcept
		{
			return m_deactivated_publisher.channel();
		}

	private:
		threshold_function_type m_threshold_function;
		bool m_active{false};
	
		action_activated_event m_activated_event{this};
		action_active_event m_active_event{this, 0.0f};
		action_deactivated_event m_deactivated_event{this};
	
		event::publisher<action_activated_event> m_activated_publisher;
		event::publisher<action_active_event> m_active_publisher;
		event::publisher<action_deactivated_event> m_deactivated_publisher;
	};
}

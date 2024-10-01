// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_INPUT_ACTION_EVENTS_HPP
#define ANTKEEPER_INPUT_ACTION_EVENTS_HPP

namespace input {

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

} // namespace input

#endif // ANTKEEPER_INPUT_ACTION_EVENTS_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.utility.state_machine;
export import <memory>;
export import <stack>;

/// Hierarchical State Machine (HSM)
export namespace engine::hsm
{
	/// Stack-based hierarchical state machine.
	/// @tparam T State type.
	template <typename T>
	using state_machine = std::stack<std::unique_ptr<T>>;
}

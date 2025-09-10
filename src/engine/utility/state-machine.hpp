// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <stack>

/// Hierarchical State Machine (HSM)
namespace engine::hsm
{
	/// Stack-based hierarchical state machine.
	/// @tparam T State type.
	template <typename T>
	using state_machine = std::stack<std::unique_ptr<T>>;
}

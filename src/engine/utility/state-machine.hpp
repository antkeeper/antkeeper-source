// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_UTILITY_HSM_STATE_MACHINE_HPP
#define ANTKEEPER_UTILITY_HSM_STATE_MACHINE_HPP

#include <memory>
#include <stack>

/// Hierarchical State Machine (HSM)
namespace hsm {

/**
 * Stack-based hierarchical state machine.
 *
 * @tparam T State type.
 */
template <typename T>
using state_machine = std::stack<std::unique_ptr<T>>;

} // namespace hsm

#endif // ANTKEEPER_UTILITY_HSM_STATE_MACHINE_HPP

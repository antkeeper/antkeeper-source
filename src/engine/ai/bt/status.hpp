// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_AI_BT_STATUS_HPP
#define ANTKEEPER_AI_BT_STATUS_HPP

namespace ai {
namespace bt {

/// Behavior tree node return status enumerations.
enum class status
{
	/// Indicates a node's execution has failed.
	failure,
	
	/// Indicates a node's execution has succeeded.
	success,
	
	/// Indicates a node's execution has not finished.
	running
};

} // namespace bt
} // namespace ai

#endif // ANTKEEPER_AI_BT_STATUS_HPP

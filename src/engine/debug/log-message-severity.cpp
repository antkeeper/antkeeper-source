// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.debug.log_message_severity;

namespace engine::debug
{
	const char* log_message_severity_to_string(log_message_severity severity) noexcept
	{
		switch (severity)
		{
			case log_message_severity::trace:
				return "trace";
			case log_message_severity::debug:
				return "debug";
			case log_message_severity::info:
				return "info";
			case log_message_severity::warning:
				return "warning";
			case log_message_severity::error:
				return "error";
			case log_message_severity::fatal:
				return "fatal";
			default:
				return "unknown";
		}
	}
}

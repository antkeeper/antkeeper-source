// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.debug.log;
import engine.debug.log_message_severity;
import engine.debug.logger;
export import <format>;
export import <source_location>;
import <utility>;

namespace engine::debug
{
	/// @name Logging
	/// @{

	/// Minimum log message severity.
	inline constexpr auto g_min_log_message_severity =
	#if !defined(NDEBUG)
		log_message_severity::trace;
	#else
		log_message_severity::debug;
	#endif

	/// Log message that formats and logs itself to the default logger on construction.
	/// @tparam Severity Message severity. A message will not be logged if @p Severity is less than min_log_message_severity.
	/// @tparam Args Types of arguments to be formatted.
	template <log_message_severity Severity, class... Args>
	struct log_message
	{
		/// Formats and logs a message.
		/// @param format Message format string.
		/// @param args Arguments to be formatted.
		/// @param location Source location from which the message was logged.
		constexpr log_message
		(
			[[maybe_unused]] std::format_string<Args...> format,
			[[maybe_unused]] Args&&... args,
			[[maybe_unused]] std::source_location&& location = std::source_location::current()
		)
		{
			if constexpr (std::to_underlying(Severity) >= std::to_underlying(g_min_log_message_severity))
			{
				default_logger().log(Severity, std::format(std::move(format), std::forward<Args>(args)...), std::move(location));
			}
		}
	};

	// Enable capture of source location as a default argument following variadic format arguments using CTAD.
	template <log_message_severity Severity, class... Args>
	log_message(std::format_string<Args...>, Args&&...) -> log_message<Severity, Args...>;

	export
	{
		/// Formats and logs a trace message.
		/// @tparam Args Types of arguments to be formatted.
		template <class... Args>
		using log_trace = log_message<log_message_severity::trace, Args...>;

		/// Formats and logs a debug message.
		/// @tparam Args Types of arguments to be formatted.
		template <class... Args>
		using log_debug = log_message<log_message_severity::debug, Args...>;

		/// Formats and logs an info message.
		/// @tparam Args Types of arguments to be formatted.
		template <class... Args>
		using log_info = log_message<log_message_severity::info, Args...>;

		/// Formats and logs a warning message.
		/// @tparam Args Types of arguments to be formatted.
		template <class... Args>
		using log_warning = log_message<log_message_severity::warning, Args...>;

		/// Formats and logs an error message.
		/// @tparam Args Types of arguments to be formatted.
		template <class... Args>
		using log_error = log_message<log_message_severity::error, Args...>;

		/// Formats and logs a fatal error message.
		/// @tparam Args Types of arguments to be formatted.
		template <class... Args>
		using log_fatal = log_message<log_message_severity::fatal, Args...>;
	}

	/// @}
}

export namespace engine
{
	using debug::log_trace;
	using debug::log_debug;
	using debug::log_info;
	using debug::log_warning;
	using debug::log_error;
	using debug::log_fatal;
}

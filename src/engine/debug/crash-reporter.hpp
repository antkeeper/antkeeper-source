// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_DEBUG_CRASH_REPORTER_HPP
#define ANTKEEPER_DEBUG_CRASH_REPORTER_HPP

#include <filesystem>
#include <string>

namespace debug {

/// Generates crash reports on unhandled exceptions or `SIGABRT`.
class crash_reporter
{
public:
	/// Constructs a crash reporter.
	crash_reporter() noexcept = default;

	/// Destructs a crash reporter.
	~crash_reporter();

	/// Sets the directory in which crash reports will be generated.
	/// @param path Path to the crash report directory.
	void set_report_directory_path(const std::filesystem::path& path);

	/// Sets the crash report filename prefix.
	/// @param prefix Crash report filename prefix.
	void set_report_prefix(const std::string& prefix);

	/// Returns the path in which crash reports will be generated.
	[[nodiscard]] inline const auto& get_report_directory_path() const noexcept
	{
		return m_report_directory_path;
	}

	/// Returns the crash report filename prefix.
	[[nodiscard]] inline const auto& get_report_prefix() const noexcept
	{
		return m_report_prefix;
	}
	
private:
	crash_reporter(const crash_reporter&) = delete;
	crash_reporter(crash_reporter&&) = delete;
	crash_reporter& operator=(const crash_reporter&) = delete;
	crash_reporter& operator=(crash_reporter&&) = delete;

	std::filesystem::path m_report_directory_path;
	std::string m_report_prefix;
};

/// Sets the global crash reporter.
/// @param reporter Pointer to a crash reporter, or `nullptr` to disable crash reporting.
/// @return Pointer to the previously set crash reporter, or `nullptr` if no crash reporter was set.
/// @warning Setting a crash reporter will set the SIGABRT handler via `std::signal()`.
/// @warning Setting a crash reporter on Windows will change `abort()` behavior via `_set_abort_behavior()`.
/// @warning Setting a crash reporter on Windows will set an unhandled exception filter via `SetUnhandledExceptionFilter()`.
crash_reporter* set_crash_reporter(crash_reporter* reporter);

} // namespace debug

#endif // ANTKEEPER_DEBUG_CRASH_REPORTER_HPP

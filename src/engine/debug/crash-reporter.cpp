// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/debug/crash-reporter.hpp>
#include <engine/debug/log.hpp>
#include <engine/utility/paths.hpp>
#include <csignal>
#include <stacktrace>

#if defined(_WIN32)
	#include <windows.h>
	#include <dbghelp.h>
	#include <tchar.h>
	#include <system_error>
#endif

namespace
{
	/// Global crash reporter.
	debug::crash_reporter* g_crash_reporter = nullptr;

	#if defined(_WIN32)
		
		/// Generates a minidump file.
		/// @param path Path to the output file.
		/// @param exception_pointers Exception pointers of exception being handled.
		/// @return `ERROR_SUCCESS` on success, and an error code otherwise.
		void generate_minidump(const std::filesystem::path& path, EXCEPTION_POINTERS* exception_pointers)
		{
			auto file = CreateFileW
			(
				path.c_str(),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);

			if (file == INVALID_HANDLE_VALUE)
			{
				const std::error_code ec(GetLastError(), std::system_category());
				throw std::system_error(ec, ec.message());
			}

			const auto dump_type = MINIDUMP_TYPE
			(
				  MiniDumpNormal
				| MiniDumpWithDataSegs
				| MiniDumpWithHandleData
				| MiniDumpWithThreadInfo
				| MiniDumpWithUnloadedModules
				| MiniDumpWithIndirectlyReferencedMemory
			);

			MINIDUMP_EXCEPTION_INFORMATION exception_info{GetCurrentThreadId(), exception_pointers, TRUE};

			if (!MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, dump_type, &exception_info, nullptr, nullptr))
			{
				const std::error_code ec(GetLastError(), std::system_category());
				CloseHandle(file);
				throw std::system_error(ec, ec.message());
			}

			CloseHandle(file);
		}

		/// Handles unhandled exceptions.
		/// @param exception_pointers Exception pointers of the exception being handled.
		/// @return Exception handling status.
		/// @note Function must not return `EXCEPTION_CONTINUE_SEARCH`, otherwise `abort()` may be called, triggering the `SIGABRT` handler crash_reporter_handle_abort(), which in turn raises another exception and handles it with this function.
		LONG WINAPI crash_reporter_unhandled_exception_filter(EXCEPTION_POINTERS* exception_pointers) noexcept
		{
			try
			{
				debug::log_fatal("Unhandled exception");
				debug::log_info("Stack trace:\n{}", std::stacktrace::current());

				const auto& crash_report_directory_path = g_crash_reporter->get_report_directory_path();
				try
				{
					// Create crash report directory, if it doesn't exist
					if (std::filesystem::create_directories(crash_report_directory_path))
					{
						debug::log_debug("Created crash report directory \"{}\"", crash_report_directory_path.string());
					}
				}
				catch (const std::exception& e)
				{
					debug::log_error("Failed to create crash report directory \"{}\": {}", crash_report_directory_path.string(), e.what());
					return EXCEPTION_EXECUTE_HANDLER;
				}

				// Generate minidump
				std::filesystem::path minidump_path;
				try
				{
					const auto time = std::chrono::system_clock::now();
					const auto minidump_filename = std::format("{0}{1:%Y%m%d}T{1:%H%M%S}Z.dmp", g_crash_reporter->get_report_prefix(), std::chrono::floor<std::chrono::seconds>(time));
					minidump_path = crash_report_directory_path / minidump_filename;
					generate_minidump(minidump_path, exception_pointers);
				}
				catch (const std::exception& e)
				{
					debug::log_error("Failed to generate crash dump \"{}\": {}", minidump_path.string(), e.what());
					return EXCEPTION_EXECUTE_HANDLER;
				}
				
				debug::log_info("Generated crash dump \"{}\"", minidump_path.string());

				const int msgboxID = MessageBox(NULL, _T("An error has occurred and the application quit unexpectedly. A crash dump has been generated. Would you like to view it?"), _T("Crash Reporter"), MB_ICONERROR | MB_YESNO | MB_SYSTEMMODAL);
				if (msgboxID == IDYES)
				{
					ShellExecuteW(nullptr, L"open", minidump_path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
				}
			}
			catch (...) {}

			return EXCEPTION_EXECUTE_HANDLER;
		}

		/// Handles heap corruption exceptions.
		/// @param exception_pointers Exception pointers of the exception being handled.
		/// @return Exception handling status.
		LONG WINAPI crash_reporter_vectored_exception_handler(EXCEPTION_POINTERS* exception_pointers)
		{
			if (exception_pointers->ExceptionRecord->ExceptionCode == STATUS_HEAP_CORRUPTION)
			{
				return crash_reporter_unhandled_exception_filter(exception_pointers);
			}

			return EXCEPTION_CONTINUE_SEARCH;
		}

	#endif

	/// Handles SIGABRT signals.
	void crash_reporter_handle_abort(int)
	{
		debug::log_fatal("abort() called");

		#if defined(_WIN32)
			
			// Raise and handle exception to trigger minidump generation
			__try
			{
				RaiseException(STATUS_FATAL_APP_EXIT, 0, 0, NULL);
			}
			__except(crash_reporter_unhandled_exception_filter(GetExceptionInformation())) {}

		#else

			debug::log_info("Stack trace:\n{}", std::stacktrace::current());

		#endif
	}
}

namespace debug {

crash_reporter::~crash_reporter()
{
	if (g_crash_reporter == this)
	{
		set_crash_reporter(nullptr);
	}
}

void crash_reporter::set_report_directory_path(const std::filesystem::path& path)
{
	m_report_directory_path = path;
}

void crash_reporter::set_report_prefix(const std::string& prefix)
{
	m_report_prefix = prefix;
}

crash_reporter* set_crash_reporter(crash_reporter* reporter)
{
	if (reporter == g_crash_reporter)
	{
		return g_crash_reporter;
	}

	crash_reporter* previous_crash_reporter = g_crash_reporter;

	using signal_handler_pointer = void (*)(int);
	static signal_handler_pointer previous_abort_handler = nullptr;

	#if defined(_WIN32)
		static unsigned int previous_abort_flags = 0;
		static LPTOP_LEVEL_EXCEPTION_FILTER previous_exception_handler = nullptr;
		static PVOID vectored_exception_handler = nullptr;
	#endif

	if (reporter)
	{
		g_crash_reporter = reporter;

		#if defined(_WIN32)
			
			if (!previous_crash_reporter)
			{
				// Prevent windows from generating/reporting crash dumps and showing a message boxes when `abort()` is called
				previous_abort_flags = _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

				// Set unhandled exception handler
				previous_exception_handler = SetUnhandledExceptionFilter(crash_reporter_unhandled_exception_filter);
			}
			else
			{
				_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
				SetUnhandledExceptionFilter(crash_reporter_unhandled_exception_filter);
			}

			// Register a vectored exception handler to catch heap corruption exceptions, which are not caught by the unhandled exception filter
			vectored_exception_handler = AddVectoredExceptionHandler(1, crash_reporter_vectored_exception_handler);
			if (!vectored_exception_handler)
			{
				debug::log_warning("Failed to register vectored exception handler.");
			}

		#endif

		// Set SIGABRT signal handler
		if (!previous_crash_reporter)
		{
			previous_abort_handler = std::signal(SIGABRT, crash_reporter_handle_abort);
		}
		else
		{
			std::signal(SIGABRT, crash_reporter_handle_abort);
		}
	}
	else
	{
		// Restore previous SIGABRT signal handler
		std::signal(SIGABRT, previous_abort_handler);

		#if defined(_WIN32)
			
			// Unregister vectored exception handler
			if (vectored_exception_handler)
			{
				if (!RemoveVectoredExceptionHandler(vectored_exception_handler))
				{
					debug::log_warning("Failed to unregister vectored exception handler.");
				}
				vectored_exception_handler = nullptr;
			}

			// Restore previous unhandled exception handler
			SetUnhandledExceptionFilter(previous_exception_handler);

			// Restore previous abort behavior
			_set_abort_behavior(previous_abort_flags, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

		#endif

		g_crash_reporter = nullptr;
	}

	return previous_crash_reporter;
}

} // namespace debug

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/debug/log.hpp>

namespace debug {

logger& default_logger() noexcept
{
	static logger instance;
	return instance;
}

} // namespace debug

/*
 * Copyright (C) 2020  Christopher J. Howard
 *
 * This file is part of Antkeeper source code.
 *
 * Antkeeper source code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper source code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "timestamp.hpp"
#include <chrono>
#include <time.h>
#include <iomanip>
#include <sstream>

std::string timestamp()
{
	auto now = std::chrono::system_clock::now();
	std::time_t tt = std::chrono::system_clock::to_time_t(now);
	std::size_t ms = (std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000).count();

	#if defined(_WIN32)
		struct std::tm timeinfo;
		localtime_s(&timeinfo, &tt);
		std::stringstream stream;
		stream << std::put_time(&timeinfo, "%Y%m%d-%H%M%S-");
		stream << std::setfill('0') << std::setw(3) << ms;
	#else
		struct std::tm* timeinfo = localtime(&tt);
		std::stringstream stream;
		stream << std::put_time(timeinfo, "%Y%m%d-%H%M%S-");
		stream << std::setfill('0') << std::setw(3) << ms;
	#endif
	
	return stream.str();
}


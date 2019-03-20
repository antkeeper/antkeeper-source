/*
 * Copyright (C) 2017-2019  Christopher J. Howard
 *
 * This file is part of Antkeeper Source Code.
 *
 * Antkeeper Source Code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Antkeeper Source Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Antkeeper Source Code.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "logger.hpp"
#include "ansi-escape-codes.hpp"
#include <iostream>

Logger::Logger():
	os(&std::cout),
	logPrefix(std::string()),
	logPostfix("\n"),
	warningPrefix(ANSI_CODE_YELLOW + std::string("Warning: ")),
	warningPostfix(ANSI_CODE_RESET),
	errorPrefix(ANSI_CODE_RED + std::string("Error: ")),
	errorPostfix(ANSI_CODE_RESET),
	successPrefix(ANSI_CODE_GREEN + std::string("Success: ")),
	successPostfix(ANSI_CODE_RESET)
{}

Logger::~Logger()
{}

void Logger::redirect(std::ostream* stream)
{
	os = stream;
}

void Logger::log(const std::string& text)
{
	if (os)
	{
		(*os) << (logPrefix + text + logPostfix);
		os->flush();
	}
}

void Logger::warning(const std::string& text)
{
	log(warningPrefix + text + warningPostfix);
}

void Logger::error(const std::string& text)
{
	log(errorPrefix + text + errorPostfix);
}

void Logger::success(const std::string& text)
{
	log(successPrefix + text + successPostfix);
}

void Logger::setLogPrefix(const std::string& prefix)
{
	logPrefix = prefix;
}

void Logger::setLogPostfix(const std::string& postfix)
{
	logPostfix = postfix;
}

void Logger::setWarningPrefix(const std::string& prefix)
{
	warningPrefix = prefix;
}

void Logger::setWarningPostfix(const std::string& postfix)
{
	warningPostfix = postfix;
}

void Logger::setErrorPrefix(const std::string& prefix)
{
	errorPrefix = prefix;
}

void Logger::setErrorPostfix(const std::string& postfix)
{
	errorPostfix = postfix;
}

void Logger::setSuccessPrefix(const std::string& prefix)
{
	successPrefix = prefix;
}

void Logger::setSuccessPostfix(const std::string& postfix)
{
	successPostfix = postfix;
}


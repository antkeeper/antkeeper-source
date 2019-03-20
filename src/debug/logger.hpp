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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <ostream>
#include <string>

class Logger
{
public:
	Logger();
	~Logger();

	/**
	 * Redirects log output to the specified output stream.
	 *
	 * @param stream Output stream to which log text will be written.
	 */
	void redirect(std::ostream* stream);
	/**
	 * Outputs text to the log.
	 */
	void log(const std::string& text);

	void warning(const std::string& text);
	void error(const std::string& text);
	void success(const std::string& text);

	void setLogPrefix(const std::string& prefix);
	void setLogPostfix(const std::string& postfix);
	void setWarningPrefix(const std::string& prefix);
	void setWarningPostfix(const std::string& postfix);
	void setErrorPrefix(const std::string& prefix);
	void setErrorPostfix(const std::string& postfix);
	void setSuccessPrefix(const std::string& prefix);
	void setSuccessPostfix(const std::string& postfix);

private:
	std::ostream* os;
	std::string logPrefix;
	std::string logPostfix;
	std::string warningPrefix;
	std::string warningPostfix;
	std::string errorPrefix;
	std::string errorPostfix;
	std::string successPrefix;
	std::string successPostfix;
};

#endif // LOGGER_HPP


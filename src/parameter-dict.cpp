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

#include "parameter-dict.hpp"
#include <sstream>

void ParameterDict::unset(const std::string& name)
{
	auto it = parameters.find(name);
	if (it != parameters.end())
	{
		parameters.erase(it);
	}
}

template <>
std::string ParameterDict::toString<std::string>(const std::string& value)
{
	return value;
}

template <>
std::string ParameterDict::toString<bool>(const bool& value)
{
	return std::to_string(static_cast<int>(value));
}

template <>
std::string ParameterDict::toString<char>(const char& value)
{
	return std::to_string(static_cast<int>(value));
}

template <>
std::string ParameterDict::toString<unsigned char>(const unsigned char& value)
{
	return std::to_string(static_cast<int>(value));
}

template <>
std::string ParameterDict::toString<int>(const int& value)
{
	return std::to_string(value);
}

template <>
std::string ParameterDict::toString<unsigned int>(const unsigned int& value)
{
	return std::to_string(value);
}

template <>
std::string ParameterDict::toString<long>(const long& value)
{
	return std::to_string(value);
}

template <>
std::string ParameterDict::toString<unsigned long>(const unsigned long& value)
{
	return std::to_string(value);
}

template <>
std::string ParameterDict::toString<float>(const float& value)
{
	return std::to_string(value);
}

template <>
std::string ParameterDict::toString<double>(const double& value)
{
	return std::to_string(value);
}

template<>
std::string ParameterDict::fromString<std::string>(const std::string& string)
{
	return string;
}

template<>
bool ParameterDict::fromString<bool>(const std::string& string)
{
	return static_cast<bool>(std::stoi(string));
}

template<>
char ParameterDict::fromString<char>(const std::string& string)
{
	return static_cast<char>(std::stoi(string));
}

template<>
unsigned char ParameterDict::fromString<unsigned char>(const std::string& string)
{
	return static_cast<unsigned char>(std::stoi(string));
}

template<>
int ParameterDict::fromString<int>(const std::string& string)
{
	return std::stoi(string);
}

template<>
unsigned int ParameterDict::fromString<unsigned int>(const std::string& string)
{
	return static_cast<unsigned int>(std::stoul(string));
}

template<>
long ParameterDict::fromString<long>(const std::string& string)
{
	return std::stol(string);
}

template<>
unsigned long ParameterDict::fromString<unsigned long>(const std::string& string)
{
	return std::stoul(string);
}

template<>
float ParameterDict::fromString<float>(const std::string& string)
{
	return std::stof(string);
}

template<>
double ParameterDict::fromString<double>(const std::string& string)
{
	return std::stod(string);
}

std::vector<std::string> ParameterDict::tokenize(const std::string& string)
{
	std::istringstream stream(string);
	std::string token;
	std::vector<std::string> tokens;
	const char delimeter = ' ';

	while (std::getline(stream, token, delimeter))
	{
		tokens.push_back(token);
	}

	return tokens;
}


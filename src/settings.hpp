/*
 * Copyright (C) 2017  Christopher J. Howard
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

#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <map>
#include <string>
#include <sstream>

class ParameterDict
{
public:
	template <typename T>
	bool get(const std::string& name, T* value) const;
	
	template <typename T>
	void set(const std::string& name, const T& value);
	
	bool load(const std::string& filename);
	bool save(const std::string& filename);

private:
	std::map<std::string, std::string> parameters;
};

template <typename T>
bool ParameterDict::get(const std::string& name, T* value) const
{
	auto it = parameters.find(name);
	if (it == parameters.end())
		return false;
	
	std::stringstream stream(it->second);
	stream >> (*value);
	
	if (stream.fail())
		return false;
	
	return true;
}

template <>
inline bool ParameterDict::get<std::string>(const std::string& name, std::string* value) const
{
	auto it = parameters.find(name);
	if (it == parameters.end())
		return false;
	
	*value = it->second;
	
	return true;
}

template <typename T>
void ParameterDict::set(const std::string& name, const T& value)
{
	std::stringstream stream;
	stream << value;
	parameters[name] = stream.str();
}

#endif


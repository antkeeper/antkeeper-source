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

#ifndef CONFIG_FILE_HPP
#define CONFIG_FILE_HPP

#include <sstream>
#include <string>
#include <unordered_map>

class config_file
{
public:
	template <typename T>
	void set(const std::string& name, const T& value);
	
	template <typename T>
	T get(const std::string& name) const;
	
	bool has(const std::string& name) const;
	
private:
	std::unordered_map<std::string, std::string> variables;
};

template <typename T>
void config_file::set(const std::string& name, const T& value)
{
	std::stringstream stream;
	stream << value;
	variables[name] = stream.str();
}

template <typename T>
T config_file::get(const std::string& name) const
{
	T value;
	if (auto it = variables.find(name); it != variables.end())
		std::stringstream(it->second) >> value;
    return value;
}

inline bool config_file::has(const std::string& name) const
{
	return (variables.find(name) != variables.end());
}

#endif // CONFIG_FILE_HPP

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

#include <codecvt>
#include <locale>
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
	void clear();
	
	const std::map<std::string, std::string>* getParameters() const;

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

template <>
inline bool ParameterDict::get<std::u32string>(const std::string& name, std::u32string* value) const
{
	auto it = parameters.find(name);
	if (it == parameters.end())
		return false;
	
	// Convert UTF-8 string to UTF-32 string
	#if _MSC_VER >= 1900
		//std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> convert;
		//*value = convert.from_bytes(it->second);
		std::wstring_convert<std::codecvt_utf8<uint32_t>, uint32_t> convert;
		auto uint32string = convert.from_bytes(it->second);
		
		value->resize(uint32string.size());
		for (std::size_t i = 0; i < uint32string.size(); ++i)
		{
			(*value)[i] = static_cast<char32_t>(uint32string[i]);
		}
	#else
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
		*value = convert.from_bytes(it->second);
	#endif
	
	return true;
}

template <typename T>
void ParameterDict::set(const std::string& name, const T& value)
{
	std::stringstream stream;
	stream << value;
	parameters[name] = stream.str();
}

inline const std::map<std::string, std::string>*ParameterDict:: getParameters() const
{
	return &parameters;
}

#endif


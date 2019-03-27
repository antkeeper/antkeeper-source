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

#ifndef PARAMETER_DICT_HPP
#define PARAMETER_DICT_HPP

#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>

/**
 * A dictionary of parameters. Parameter values are stored as strings.
 */
class ParameterDict
{
public:
	/**
	 * Sets the value of a parameter. This value will be stored as a string.
	 *
	 * @tparam T Parameter value type.
	 * @param name Name of the parameter.
	 * @param value Value of the parameter.
	 */
	template<class T>
	void set(const std::string& name, const T& value);

	/**
	 * Sets the values of an array parameter. These values will be stored as a string.
	 *
	 * @tparam T Array element type.
	 * @tparam N Size of the array.
	 * @param name Name of the parameter.
	 * @param values Array of values.
	 */
	template <class T, std::size_t N>
	void set(const std::string& name, const std::array<T, N>& values);

	/**
	 * Removes a parameter from the parameter dict.
	 *
	 * @param name Name of the parameter to be removed.
	 */
	void unset(const std::string& name);
	
	/**
	 * Returns the value of a parameter. If the parameter has not been set, std::nullopt will be returned.
	 *
	 * @tparam T Parameter value type.
	 * @param name Name of the parameter.
	 */
	template<class T>
	std::optional<T> get(const std::string& name);

	/**
	 * Returns the values of an array parameter. If the parameter has not been set, std::nyllopt will be returned.
	 *
	 * @tparam T Array element type.
	 * @tparam N Size of the array.
	 * @param name Name of the parameter.
	 */
	template <class T, std::size_t N>
	std::optional<std::array<T, N>> get(const std::string& name);

	/// Returns all parameters in the dict.
	const std::map<std::string, std::string>* getParameters() const;
	
private:
	/// Converts a value to a string.
	template <class T>
	static std::string toString(const T& value);

	/// Converts a string to a value.
	template <class T>
	static T fromString(const std::string& string);

	/// Splits a string into space-delimeted tokens
	static std::vector<std::string> tokenize(const std::string& string);

	std::map<std::string, std::string> parameters;
};

template <class T>
void ParameterDict::set(const std::string& name, const T& value)
{
	parameters[name] = toString<T>(value);
}

template <class T, std::size_t N>
void ParameterDict::set(const std::string& name, const std::array<T, N>& values)
{
	std::string string;
	for (std::size_t i = 0; i < N; ++i)
	{
		string += toString<T>(values[i]);

		if (i < N - 1)
		{
			string += ' ';
		}
	}

	parameters[name] = string;
}

template <class T>
std::optional<T> ParameterDict::get(const std::string& name)
{
	auto it = parameters.find(name);
	if (it == parameters.end())
	{
		return std::nullopt;
	}

	return fromString<T>(it->second);
}

template <class T, std::size_t N>
std::optional<std::array<T, N>> ParameterDict::get(const std::string& name)
{
	auto it = parameters.find(name);
	if (it == parameters.end())
	{
		return std::nullopt;
	}

	// Create array to hold values
	std::array<T, N> values;

	// Tokenize string
	std::vector<std::string> tokens = tokenize(it->second);
	for (std::size_t i = 0; i < tokens.size(); ++i)
	{
		if (i >= values.size())
		{
			break;
		}

		// Convert token to value
		values[i] = fromString<T>(tokens[i]);
	}

	return values;
}

inline const std::map<std::string, std::string>* ParameterDict::getParameters() const
{
	return &parameters;
}

#endif // PARAMETER_DICT_HPP


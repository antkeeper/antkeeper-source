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

#ifndef ANTKEEPER_MATERIAL_PROPERTY_HPP
#define ANTKEEPER_MATERIAL_PROPERTY_HPP

#include "rasterizer/shader-variable-type.hpp"
#include "rasterizer/shader-input.hpp"
#include <vmq/vmq.hpp>
#include <cstdlib>

using namespace vmq::types;

class material;
class shader_program;
class texture_2d;
class texture_cube;

/**
 * Abstract base class for material properties.
 */
class material_property_base
{
public:
	/**
	 * Connects the material property to a shader input.
	 *
	 * @param input Shader input to which the material property should be connected.
	 * @return `true` if the property was connected to the input successfully, `false` otherwise.
	 */
	bool connect(const shader_input* input);

	/**
	 * Disconnects the material property from its shader input.
	 */
	void disconnect();

	/**
	 * Uploads the material property to its shader program.
	 *
	 * @return `true` if the property was uploaded successfully, `false` otherwise.
	 */
	virtual bool upload() const = 0;

	/**
	 * Returns the type of data which the property contains.
	 */
	virtual shader_variable_type get_data_type() const = 0;

	/**
	 * Returns `true` if the material property is connected to a shader input, `false` otherwise.
	 */
	bool is_connected() const;

	/**
	 * Creates a copy of this material property.
	 */
	virtual material_property_base* clone() const = 0;

protected:
	material_property_base();

	const shader_input* input;
};

inline bool material_property_base::is_connected() const
{
	return (input != nullptr);
}

/**
 * A property of a material which can be uploaded to a shader program via a shader input.
 *
 * @tparam T Property data type.
 */
template <class T>
class material_property: public material_property_base
{
public:
	/**
	 * Creates a material property.
	 *
	 * @param element_count Number of elements in the property array.
	 */
	material_property(std::size_t element_count);

	/**
	 * Destroys a material property.
	 */
	virtual ~material_property();

	material_property(const material_property<T>&) = delete;
	material_property<T>& operator=(const material_property<T>&) = delete;

	/// @copydoc material_property_base::upload() const
	virtual bool upload() const;

	/**
	 * Sets the value of this property.
	 *
	 * @param value Value to set.
	 */
	void set_value(const T& value);
	
	/**
	 * Sets the value of a single element in this array property.
	 *
	 * @param index Index of an array element.
	 * @param value Value to set.
	 */
	void set_value(std::size_t index, const T& value);
	
	/**
	 * Sets the values of a range of elements in this array property.
	 *
	 * @param index Index of the first array element to set.
	 * @param values Pointer to an array of values to set.
	 * @param count Number of elements to set.
	 */
	void set_values(std::size_t index, const T* values, std::size_t count);

	/// @copydoc material_property_base::get_data_type() const
	virtual shader_variable_type get_data_type() const;

	/// @copydoc material_property_base::clone() const
	virtual material_property_base* clone() const;

private:
	std::size_t element_count;
	T* values;
};

template <class T>
material_property<T>::material_property(std::size_t element_count):
	element_count(element_count),
	values(nullptr)
{
	values = new T[element_count];
}

template <class T>
material_property<T>::~material_property()
{
	delete[] values;
}

template <class T>
bool material_property<T>::upload() const
{
	if (!is_connected())
	{
		return false;
	}
	
	if (element_count > 1)
	{
		return input->upload(0, values, element_count);
	}
	else
	{
		return input->upload(values[0]);
	}
}

template <class T>
void material_property<T>::set_value(const T& value)
{
	values[0] = value;
}

template <class T>
void material_property<T>::set_value(std::size_t index, const T& value)
{
	values[index] = value;
}

template <class T>
void material_property<T>::set_values(std::size_t index, const T* values, std::size_t count)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		this->values[index + i] = values[i];
	}
}

template <>
inline shader_variable_type material_property<bool>::get_data_type() const
{
	return shader_variable_type::bool1;
}

template <>
inline shader_variable_type material_property<bool2>::get_data_type() const
{
	return shader_variable_type::bool2;
}

template <>
inline shader_variable_type material_property<bool3>::get_data_type() const
{
	return shader_variable_type::bool3;
}

template <>
inline shader_variable_type material_property<bool4>::get_data_type() const
{
	return shader_variable_type::bool4;
}

template <>
inline shader_variable_type material_property<int>::get_data_type() const
{
	return shader_variable_type::int1;
}

template <>
inline shader_variable_type material_property<int2>::get_data_type() const
{
	return shader_variable_type::int2;
}

template <>
inline shader_variable_type material_property<int3>::get_data_type() const
{
	return shader_variable_type::int3;
}

template <>
inline shader_variable_type material_property<int4>::get_data_type() const
{
	return shader_variable_type::int4;
}

template <>
inline shader_variable_type material_property<unsigned int>::get_data_type() const
{
	return shader_variable_type::uint1;
}

template <>
inline shader_variable_type material_property<uint2>::get_data_type() const
{
	return shader_variable_type::uint2;
}

template <>
inline shader_variable_type material_property<uint3>::get_data_type() const
{
	return shader_variable_type::uint3;
}

template <>
inline shader_variable_type material_property<uint4>::get_data_type() const
{
	return shader_variable_type::uint4;
}

template <>
inline shader_variable_type material_property<float>::get_data_type() const
{
	return shader_variable_type::float1;
}

template <>
inline shader_variable_type material_property<float2>::get_data_type() const
{
	return shader_variable_type::float2;
}

template <>
inline shader_variable_type material_property<float3>::get_data_type() const
{
	return shader_variable_type::float3;
}

template <>
inline shader_variable_type material_property<float4>::get_data_type() const
{
	return shader_variable_type::float4;
}

template <>
inline shader_variable_type material_property<float2x2>::get_data_type() const
{
	return shader_variable_type::float2x2;
}

template <>
inline shader_variable_type material_property<float3x3>::get_data_type() const
{
	return shader_variable_type::float3x3;
}

template <>
inline shader_variable_type material_property<float4x4>::get_data_type() const
{
	return shader_variable_type::float4x4;
}

template <>
inline shader_variable_type material_property<const texture_2d*>::get_data_type() const
{
	return shader_variable_type::texture_2d;
}

template <>
inline shader_variable_type material_property<const texture_cube*>::get_data_type() const
{
	return shader_variable_type::texture_cube;
}

template <class T>
material_property_base* material_property<T>::clone() const
{
	material_property<T>* property = new material_property<T>(element_count);
	for (std::size_t i = 0; i < element_count; ++i)
	{
		property->values[i] = values[i];
	}
	property->input = input;

	return property;
}

#endif // ANTKEEPER_MATERIAL_PROPERTY_HPP


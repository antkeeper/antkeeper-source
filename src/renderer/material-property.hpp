/*
 * Copyright (C) 2021  Christopher J. Howard
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

#include "animation/tween.hpp"
#include "gl/shader-variable-type.hpp"
#include "gl/shader-input.hpp"
#include "math/interpolation.hpp"
#include "utility/fundamental-types.hpp"
#include "gl/shader-program.hpp"
#include "gl/texture-2d.hpp"
#include "gl/texture-cube.hpp"
#include <cstddef>

class material;

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
	bool connect(const gl::shader_input* input);

	/**
	 * Disconnects the material property from its shader input.
	 */
	void disconnect();
	
	/**
	 * Sets state 0 = state 1.
	 */
	virtual void update_tweens() = 0;

	/**
	 * Uploads the material property to its shader program.
	 *
	 * @param a Interpolation factor. Should be on `[0.0, 1.0]`.
	 * @return `true` if the property was uploaded successfully, `false` otherwise.
	 */
	virtual bool upload(double a) const = 0;

	/**
	 * Returns the type of data which the property contains.
	 */
	virtual gl::shader_variable_type get_data_type() const = 0;

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

	const gl::shader_input* input;
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
	typedef tween<T> tween_type;
	typedef typename tween<T>::interpolator_type interpolator_type;
	
	/// Default tween interpolator function for this material property type.
	static T default_interpolator(const T& x, const T& y, double a);
	
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
	
	/// @copydoc material_property_base::update_tweens()
	virtual void update_tweens();

	/// @copydoc material_property_base::upload() const
	virtual bool upload(double a) const;

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
	
	/**
	 * Sets the tween interpolator function.
	 *
	 * @param interpolator Tween interpolator function.
	 */
	void set_tween_interpolator(interpolator_type interpolator);
	
	/// Returns the value of the first element in this property.
	const T& get_value() const;
	
	/**
	 * Returns the value of the first element in this property.
	 *
	 * @param index Index of an array element.
	 * @return Value of the element at the specified index.
	 */
	const T& get_value(std::size_t index) const;

	/// @copydoc material_property_base::get_data_type() const
	virtual gl::shader_variable_type get_data_type() const;

	/// @copydoc material_property_base::clone() const
	virtual material_property_base* clone() const;

private:
	std::size_t element_count;
	tween<T>* values;
};

template <typename T>
inline T material_property<T>::default_interpolator(const T& x, const T& y, double a)
{
	return y;
}

template <>
inline float material_property<float>::default_interpolator(const float& x, const float& y, double a)
{
	return math::lerp<float, float>(x, y, static_cast<float>(a));
}

template <>
inline float2 material_property<float2>::default_interpolator(const float2& x, const float2& y, double a)
{
	return math::lerp<float2, float>(x, y, static_cast<float>(a));
}

template <>
inline float3 material_property<float3>::default_interpolator(const float3& x, const float3& y, double a)
{
	return math::lerp<float3, float>(x, y, static_cast<float>(a));
}

template <>
inline float4 material_property<float4>::default_interpolator(const float4& x, const float4& y, double a)
{
	return math::lerp<float4, float>(x, y, static_cast<float>(a));
}

template <class T>
material_property<T>::material_property(std::size_t element_count):
	element_count(element_count),
	values(nullptr)
{
	values = new tween<T>[element_count];
	set_tween_interpolator(default_interpolator);
}

template <class T>
material_property<T>::~material_property()
{
	delete[] values;
}

template <class T>
void material_property<T>::update_tweens()
{
	for (std::size_t i = 0; i < element_count; ++i)
	{
		values[i].update();
	}
}

template <class T>
bool material_property<T>::upload(double a) const
{
	if (!is_connected())
	{
		return false;
	}
	
	if (element_count > 1)
	{
		for (std::size_t i = 0; i < element_count; ++i)
		{
			if (!input->upload(i, values[i].interpolate(a)))
				return false;
		}
		
		return true;
	}
	else
	{
		return input->upload(values[0].interpolate(a));
	}
}

template <class T>
void material_property<T>::set_value(const T& value)
{
	values[0][1] = value;
}

template <class T>
void material_property<T>::set_value(std::size_t index, const T& value)
{
	values[index][1] = value;
}

template <class T>
void material_property<T>::set_values(std::size_t index, const T* values, std::size_t count)
{
	for (std::size_t i = 0; i < count; ++i)
	{
		this->values[index + i][1] = values[i];
	}
}

template <class T>
void material_property<T>::set_tween_interpolator(interpolator_type interpolator)
{
	for (std::size_t i = 0; i < element_count; ++i)
	{
		this->values[i].set_interpolator(interpolator);
	}
}

template <class T>
inline const T& material_property<T>::get_value() const
{
	return values[0][1];
}

template <class T>
inline const T& material_property<T>::get_value(std::size_t index) const
{
	return values[index][1];
}

template <>
inline gl::shader_variable_type material_property<bool>::get_data_type() const
{
	return gl::shader_variable_type::bool1;
}

template <>
inline gl::shader_variable_type material_property<bool2>::get_data_type() const
{
	return gl::shader_variable_type::bool2;
}

template <>
inline gl::shader_variable_type material_property<bool3>::get_data_type() const
{
	return gl::shader_variable_type::bool3;
}

template <>
inline gl::shader_variable_type material_property<bool4>::get_data_type() const
{
	return gl::shader_variable_type::bool4;
}

template <>
inline gl::shader_variable_type material_property<int>::get_data_type() const
{
	return gl::shader_variable_type::int1;
}

template <>
inline gl::shader_variable_type material_property<int2>::get_data_type() const
{
	return gl::shader_variable_type::int2;
}

template <>
inline gl::shader_variable_type material_property<int3>::get_data_type() const
{
	return gl::shader_variable_type::int3;
}

template <>
inline gl::shader_variable_type material_property<int4>::get_data_type() const
{
	return gl::shader_variable_type::int4;
}

template <>
inline gl::shader_variable_type material_property<unsigned int>::get_data_type() const
{
	return gl::shader_variable_type::uint1;
}

template <>
inline gl::shader_variable_type material_property<uint2>::get_data_type() const
{
	return gl::shader_variable_type::uint2;
}

template <>
inline gl::shader_variable_type material_property<uint3>::get_data_type() const
{
	return gl::shader_variable_type::uint3;
}

template <>
inline gl::shader_variable_type material_property<uint4>::get_data_type() const
{
	return gl::shader_variable_type::uint4;
}

template <>
inline gl::shader_variable_type material_property<float>::get_data_type() const
{
	return gl::shader_variable_type::float1;
}

template <>
inline gl::shader_variable_type material_property<float2>::get_data_type() const
{
	return gl::shader_variable_type::float2;
}

template <>
inline gl::shader_variable_type material_property<float3>::get_data_type() const
{
	return gl::shader_variable_type::float3;
}

template <>
inline gl::shader_variable_type material_property<float4>::get_data_type() const
{
	return gl::shader_variable_type::float4;
}

template <>
inline gl::shader_variable_type material_property<float2x2>::get_data_type() const
{
	return gl::shader_variable_type::float2x2;
}

template <>
inline gl::shader_variable_type material_property<float3x3>::get_data_type() const
{
	return gl::shader_variable_type::float3x3;
}

template <>
inline gl::shader_variable_type material_property<float4x4>::get_data_type() const
{
	return gl::shader_variable_type::float4x4;
}

template <>
inline gl::shader_variable_type material_property<const gl::texture_2d*>::get_data_type() const
{
	return gl::shader_variable_type::texture_2d;
}

template <>
inline gl::shader_variable_type material_property<const gl::texture_cube*>::get_data_type() const
{
	return gl::shader_variable_type::texture_cube;
}

template <class T>
material_property_base* material_property<T>::clone() const
{
	material_property<T>* property = new material_property<T>(element_count);
	for (std::size_t i = 0; i < element_count; ++i)
	{
		property->values[i][0] = values[i][0];
		property->values[i][1] = values[i][1];
	}
	property->input = input;

	return property;
}

#endif // ANTKEEPER_MATERIAL_PROPERTY_HPP


/*
 * Copyright (C) 2023  Christopher J. Howard
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

#ifndef ANTKEEPER_RENDER_MATERIAL_VARIABLE_HPP
#define ANTKEEPER_RENDER_MATERIAL_VARIABLE_HPP

#include <engine/utility/fundamental-types.hpp>
#include <engine/gl/texture-1d.hpp>
#include <engine/gl/texture-2d.hpp>
#include <engine/gl/texture-3d.hpp>
#include <engine/gl/texture-cube.hpp>
#include <engine/render/material-variable-type.hpp>
#include <memory>
#include <vector>

namespace render {

/**
 * Abstract base class for material variables.
 */
class material_variable_base
{
public:
	/**
	 * Destructs a material variable base.
	 */
	virtual ~material_variable_base() = default;
	
	/**
	 * Returns the material variable data type.
	 */
	[[nodiscard]] virtual constexpr material_variable_type type() const noexcept = 0;
	
	/**
	 * Returns the number of elements in an array variable, or `1` if the variable is not an array.
	 */
	[[nodiscard]] virtual std::size_t size() const noexcept = 0;
	
	/**
	 * Creates a copy of this material property.
	 */
	[[nodiscard]] virtual std::unique_ptr<material_variable_base> clone() const = 0;
};

/**
 * Material variable.
 *
 * @tparam T Material variable value type.
 */
template <class T>
class material_variable: public material_variable_base
{
public:
	/// Material variable element type.
	using element_type = T;
	
	/**
	 * Constructs a material variable.
	 *
	 * @param size Number of elements in the array, or `1` if the variable is not an array.
	 * @param value Value with which to initialize the elements.
	 */
	inline material_variable(std::size_t size, const element_type& value = element_type()):
		elements(size, value)
	{}
	
	/**
	 * Constructs a material variable with a single element.
	 */
	inline material_variable():
		material_variable(1)
	{}
	
	/**
	 * Constructs a material variable from a list of element values.
	 *
	 * @param list List of element values.
	 */
	inline material_variable(std::initializer_list<element_type> list):
		elements(list)
	{}
	
	[[nodiscard]] virtual constexpr material_variable_type type() const noexcept override;
	
	[[nodiscard]] inline std::size_t size() const noexcept override
	{
		return elements.size();
	}
	
	[[nodiscard]] inline std::unique_ptr<material_variable_base> clone() const override
	{
		return std::make_unique<material_variable<T>>(*this);
	}
	
	/**
	 * Sets the value of the the variable, or the value of the first element if the variable is an array.
	 *
	 * @param value Value to set.
	 */
	inline void set(const element_type& value)
	{
		elements.front() = value;
	}
	
	/**
	 * Sets the value of a single element in an array variable.
	 *
	 * @param index Index of an element.
	 * @param value Value to set.
	 */
	inline void set(std::size_t index, const element_type& value)
	{
		elements[index] = value;
	}
	
	/**
	 * Returns a reference to the first element in the array.
	 */
	[[nodiscard]] inline const element_type& get() const
	{
		return elements.front();
	}
	
	/**
	 * Returns a reference to the element at a given index.
	 *
	 * @param index Index of an element.
	 *
	 * @return Reference to the element at @p index.
	 */
	[[nodiscard]] inline const element_type& get(std::size_t index) const
	{
		return elements[index];
	}
	
	/**
	 * Returns a pointer to the element array.
	 */
	[[nodiscard]] inline const element_type* data() const noexcept
	{
		return elements.data();
	}
	
private:
	std::vector<element_type> elements;
};

/// Boolean material variable.
using material_bool = material_variable<bool>;

/// 2-dimensional boolean vector material variable.
using material_bool2 = material_variable<bool2>;

/// 3-dimensional boolean vector material variable.
using material_bool3 = material_variable<bool3>;

/// 4-dimensional boolean vector material variable.
using material_bool4 = material_variable<bool4>;

/// Integer material variable.
using material_int = material_variable<int>;

/// 2-dimensional integer vector material variable.
using material_int2 = material_variable<int2>;

/// 3-dimensional integer vector material variable.
using material_int3 = material_variable<int3>;

/// 4-dimensional integer vector material variable.
using material_int4 = material_variable<int4>;

/// Unsigned integer material variable.
using material_uint = material_variable<unsigned int>;

/// 2-dimensional unsigned integer vector material variable.
using material_uint2 = material_variable<uint2>;

/// 3-dimensional unsigned integer vector material variable.
using material_uint3 = material_variable<uint3>;

/// 4-dimensional unsigned integer vector material variable.
using material_uint4 = material_variable<uint4>;

/// Floating-point material variable.
using material_float = material_variable<float>;

/// 2-dimensional floating-point vector material variable.
using material_float2 = material_variable<float2>;

/// 3-dimensional floating-point vector material variable.
using material_float3 = material_variable<float3>;

/// 4-dimensional floating-point vector material variable.
using material_float4 = material_variable<float4>;

/// 2x2 floating-point matrix material variable.
using material_float2x2 = material_variable<float2x2>;

/// 3x3 floating-point matrix material variable.
using material_float3x3 = material_variable<float3x3>;

/// 4x4 floating-point matrix material variable.
using material_float4x4 = material_variable<float4x4>;

/// 1-dimensional texture material variable.
using material_texture_1d = material_variable<std::shared_ptr<gl::texture_1d>>;

/// 2-dimensional texture material variable.
using material_texture_2d = material_variable<std::shared_ptr<gl::texture_2d>>;

/// 3-dimensional texture material variable.
using material_texture_3d = material_variable<std::shared_ptr<gl::texture_3d>>;

/// Cube texture material variable.
using material_texture_cube = material_variable<std::shared_ptr<gl::texture_cube>>;

template <>
inline constexpr material_variable_type material_bool::type() const noexcept
{
	return material_variable_type::bool1;
}

template <>
inline constexpr material_variable_type material_bool2::type() const noexcept
{
	return material_variable_type::bool2;
}

template <>
inline constexpr material_variable_type material_bool3::type() const noexcept
{
	return material_variable_type::bool3;
}

template <>
inline constexpr material_variable_type material_bool4::type() const noexcept
{
	return material_variable_type::bool4;
}

template <>
inline constexpr material_variable_type material_int::type() const noexcept
{
	return material_variable_type::int1;
}

template <>
inline constexpr material_variable_type material_int2::type() const noexcept
{
	return material_variable_type::int2;
}

template <>
inline constexpr material_variable_type material_int3::type() const noexcept
{
	return material_variable_type::int3;
}

template <>
inline constexpr material_variable_type material_int4::type() const noexcept
{
	return material_variable_type::int4;
}

template <>
inline constexpr material_variable_type material_uint::type() const noexcept
{
	return material_variable_type::uint1;
}

template <>
inline constexpr material_variable_type material_uint2::type() const noexcept
{
	return material_variable_type::uint2;
}

template <>
inline constexpr material_variable_type material_uint3::type() const noexcept
{
	return material_variable_type::uint3;
}

template <>
inline constexpr material_variable_type material_uint4::type() const noexcept
{
	return material_variable_type::uint4;
}

template <>
inline constexpr material_variable_type material_float::type() const noexcept
{
	return material_variable_type::float1;
}

template <>
inline constexpr material_variable_type material_float2::type() const noexcept
{
	return material_variable_type::float2;
}

template <>
inline constexpr material_variable_type material_float3::type() const noexcept
{
	return material_variable_type::float3;
}

template <>
inline constexpr material_variable_type material_float4::type() const noexcept
{
	return material_variable_type::float4;
}

template <>
inline constexpr material_variable_type material_float2x2::type() const noexcept
{
	return material_variable_type::float2x2;
}

template <>
inline constexpr material_variable_type material_float3x3::type() const noexcept
{
	return material_variable_type::float3x3;
}

template <>
inline constexpr material_variable_type material_float4x4::type() const noexcept
{
	return material_variable_type::float4x4;
}

template <>
inline constexpr material_variable_type material_texture_1d::type() const noexcept
{
	return material_variable_type::texture_1d;
}

template <>
inline constexpr material_variable_type material_texture_2d::type() const noexcept
{
	return material_variable_type::texture_2d;
}

template <>
inline constexpr material_variable_type material_texture_3d::type() const noexcept
{
	return material_variable_type::texture_3d;
}

template <>
inline constexpr material_variable_type material_texture_cube::type() const noexcept
{
	return material_variable_type::texture_cube;
}

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_VARIABLE_HPP

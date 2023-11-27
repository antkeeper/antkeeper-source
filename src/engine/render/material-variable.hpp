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

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/gl/texture.hpp>
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
	inline explicit material_variable(std::size_t size, const element_type& value = element_type()):
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
	inline explicit material_variable(std::initializer_list<element_type> list):
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
using matvar_bool = material_variable<bool>;

/// 2-dimensional boolean vector material variable.
using matvar_bvec2 = material_variable<math::bvec2>;

/// 3-dimensional boolean vector material variable.
using matvar_bvec3 = material_variable<math::bvec3>;

/// 4-dimensional boolean vector material variable.
using matvar_bvec4 = material_variable<math::bvec4>;

/// Integer material variable.
using matvar_int = material_variable<int>;

/// 2-dimensional integer vector material variable.
using matvar_ivec2 = material_variable<math::ivec2>;

/// 3-dimensional integer vector material variable.
using matvar_ivec3 = material_variable<math::ivec3>;

/// 4-dimensional integer vector material variable.
using matvar_ivec4 = material_variable<math::ivec4>;

/// Unsigned integer material variable.
using matvar_uint = material_variable<unsigned int>;

/// 2-dimensional unsigned integer vector material variable.
using matvar_uvec2 = material_variable<math::uvec2>;

/// 3-dimensional unsigned integer vector material variable.
using matvar_uvec3 = material_variable<math::uvec3>;

/// 4-dimensional unsigned integer vector material variable.
using matvar_uvec4 = material_variable<math::uvec4>;

/// Floating-point material variable.
using matvar_float = material_variable<float>;

/// 2-dimensional floating-point vector material variable.
using matvar_fvec2 = material_variable<math::fvec2>;

/// 3-dimensional floating-point vector material variable.
using matvar_fvec3 = material_variable<math::fvec3>;

/// 4-dimensional floating-point vector material variable.
using matvar_fvec4 = material_variable<math::fvec4>;

/// 2x2 floating-point matrix material variable.
using matvar_fmat2 = material_variable<math::fmat2>;

/// 3x3 floating-point matrix material variable.
using matvar_fmat3 = material_variable<math::fmat3>;

/// 4x4 floating-point matrix material variable.
using matvar_fmat4 = material_variable<math::fmat4>;

/// 1-dimensional texture material variable.
using matvar_texture_1d = material_variable<std::shared_ptr<gl::texture_1d>>;

/// 2-dimensional texture material variable.
using matvar_texture_2d = material_variable<std::shared_ptr<gl::texture_2d>>;

/// 3-dimensional texture material variable.
using matvar_texture_3d = material_variable<std::shared_ptr<gl::texture_3d>>;

/// Cube texture material variable.
using matvar_texture_cube = material_variable<std::shared_ptr<gl::texture_cube>>;

template <>
inline constexpr material_variable_type matvar_bool::type() const noexcept
{
	return material_variable_type::bvec1;
}

template <>
inline constexpr material_variable_type matvar_bvec2::type() const noexcept
{
	return material_variable_type::bvec2;
}

template <>
inline constexpr material_variable_type matvar_bvec3::type() const noexcept
{
	return material_variable_type::bvec3;
}

template <>
inline constexpr material_variable_type matvar_bvec4::type() const noexcept
{
	return material_variable_type::bvec4;
}

template <>
inline constexpr material_variable_type matvar_int::type() const noexcept
{
	return material_variable_type::ivec1;
}

template <>
inline constexpr material_variable_type matvar_ivec2::type() const noexcept
{
	return material_variable_type::ivec2;
}

template <>
inline constexpr material_variable_type matvar_ivec3::type() const noexcept
{
	return material_variable_type::ivec3;
}

template <>
inline constexpr material_variable_type matvar_ivec4::type() const noexcept
{
	return material_variable_type::ivec4;
}

template <>
inline constexpr material_variable_type matvar_uint::type() const noexcept
{
	return material_variable_type::uvec1;
}

template <>
inline constexpr material_variable_type matvar_uvec2::type() const noexcept
{
	return material_variable_type::uvec2;
}

template <>
inline constexpr material_variable_type matvar_uvec3::type() const noexcept
{
	return material_variable_type::uvec3;
}

template <>
inline constexpr material_variable_type matvar_uvec4::type() const noexcept
{
	return material_variable_type::uvec4;
}

template <>
inline constexpr material_variable_type matvar_float::type() const noexcept
{
	return material_variable_type::fvec1;
}

template <>
inline constexpr material_variable_type matvar_fvec2::type() const noexcept
{
	return material_variable_type::fvec2;
}

template <>
inline constexpr material_variable_type matvar_fvec3::type() const noexcept
{
	return material_variable_type::fvec3;
}

template <>
inline constexpr material_variable_type matvar_fvec4::type() const noexcept
{
	return material_variable_type::fvec4;
}

template <>
inline constexpr material_variable_type matvar_fmat2::type() const noexcept
{
	return material_variable_type::fmat2;
}

template <>
inline constexpr material_variable_type matvar_fmat3::type() const noexcept
{
	return material_variable_type::fmat3;
}

template <>
inline constexpr material_variable_type matvar_fmat4::type() const noexcept
{
	return material_variable_type::fmat4;
}

template <>
inline constexpr material_variable_type matvar_texture_1d::type() const noexcept
{
	return material_variable_type::texture_1d;
}

template <>
inline constexpr material_variable_type matvar_texture_2d::type() const noexcept
{
	return material_variable_type::texture_2d;
}

template <>
inline constexpr material_variable_type matvar_texture_3d::type() const noexcept
{
	return material_variable_type::texture_3d;
}

template <>
inline constexpr material_variable_type matvar_texture_cube::type() const noexcept
{
	return material_variable_type::texture_cube;
}

} // namespace render

#endif // ANTKEEPER_RENDER_MATERIAL_VARIABLE_HPP

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <engine/gl/texture.hpp>
#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/utility/sized-types.hpp>
#include <memory>
#include <vector>

namespace engine::render
{
	/// Material variable data types.
	enum class material_variable_type
	{
		bvec1,
		bvec2,
		bvec3,
		bvec4,
		ivec1,
		ivec2,
		ivec3,
		ivec4,
		uvec1,
		uvec2,
		uvec3,
		uvec4,
		fvec1,
		fvec2,
		fvec3,
		fvec4,
		fmat2,
		fmat3,
		fmat4,
		texture_1d,
		texture_2d,
		texture_3d,
		texture_cube
	};

	/// Abstract base class for material variables.
	class material_variable_base
	{
	public:
		/// Destructs a material variable base.
		virtual ~material_variable_base() = default;
	
		/// Returns the material variable data type.
		[[nodiscard]] virtual material_variable_type type() const noexcept = 0;
	
		/// Returns the number of elements in an array variable, or `1` if the variable is not an array.
		[[nodiscard]] virtual usize size() const noexcept = 0;
	
		/// Creates a copy of this material property.
		[[nodiscard]] virtual std::unique_ptr<material_variable_base> clone() const = 0;
	};

	/// Material variable.
	/// @tparam T Material variable value type.
	template <class T>
	class material_variable: public material_variable_base
	{
	public:
		/// Material variable element type.
		using element_type = T;
	
		/// Constructs a material variable.
		/// @param size Number of elements in the array, or `1` if the variable is not an array.
		/// @param value Value with which to initialize the elements.
		inline explicit material_variable(usize size, const element_type& value = element_type()):
			elements(size, value)
		{}
	
		/// Constructs a material variable with a single element.
		inline material_variable():
			material_variable(1)
		{}
	
		/// Constructs a material variable from a list of element values.
		/// @param list List of element values.
		inline explicit material_variable(std::initializer_list<element_type> list):
			elements(list)
		{}
	
		[[nodiscard]] material_variable_type type() const noexcept override;
	
		[[nodiscard]] inline usize size() const noexcept override
		{
			return elements.size();
		}
	
		[[nodiscard]] inline std::unique_ptr<material_variable_base> clone() const override
		{
			return std::make_unique<material_variable<T>>(*this);
		}
	
		/// Sets the value of the the variable, or the value of the first element if the variable is an array.
		/// @param value Value to set.
		inline void set(const element_type& value)
		{
			elements.front() = value;
		}
	
		/// Sets the value of a single element in an array variable.
		/// @param index Index of an element.
		/// @param value Value to set.
		inline void set(usize index, const element_type& value)
		{
			elements[index] = value;
		}
	
		/// Returns a reference to the first element in the array.
		[[nodiscard]] inline const element_type& get() const
		{
			return elements.front();
		}
	
		/// Returns a reference to the element at a given index.
		/// @param index Index of an element.
		/// @return Reference to the element at @p index.
		[[nodiscard]] inline const element_type& get(usize index) const
		{
			return elements[index];
		}
	
		/// Returns a pointer to the element array.
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
}

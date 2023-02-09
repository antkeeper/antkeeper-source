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

#ifndef ANTKEEPER_MATH_GLSL_HPP
#define ANTKEEPER_MATH_GLSL_HPP

#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "math/quaternion.hpp"

namespace math {
namespace glsl {

/**
 * Linear algebra data types with GLSL naming conventions.
 */
namespace types
{
	/// @name Vector types
	/// @{
	
	/**
	 * *n*-dimensional vector of booleans.
	 *
	 * @tparam N Number of elements
	 */
	/// @{
	template <std::size_t N>
	using bvec = math::vector<bool, N>;
	using bvec2 = bvec<2>;
	using bvec3 = bvec<3>;
	using bvec4 = bvec<4>;
	/// @}
	
	/**
	 * *n*-dimensional vector of signed integers.
	 *
	 * @tparam N Number of elements
	 */
	/// @{
	template <std::size_t N>
	using ivec = math::vector<int, N>;
	using ivec2 = ivec<2>;
	using ivec3 = ivec<3>;
	using ivec4 = ivec<4>;
	/// @}
	
	/**
	 * *n*-dimensional vector of unsigned integers.
	 *
	 * @tparam N Number of elements
	 */
	/// @{
	template <std::size_t N>
	using uvec = math::vector<unsigned int, N>;
	using uvec2 = uvec<2>;
	using uvec3 = uvec<3>;
	using uvec4 = uvec<4>;
	/// @}
	
	/**
	 * *n*-dimensional vector of floating-point numbers.
	 *
	 * @tparam N Number of elements
	 */
	/// @{
	template <std::size_t N>
	using fvec = math::vector<float, N>;
	using fvec2 = fvec<2>;
	using fvec3 = fvec<3>;
	using fvec4 = fvec<4>;
	using vec2 = fvec2;
	using vec3 = fvec3;
	using vec4 = fvec4;
	/// @}
	
	/**
	 * *n*-dimensional vector of double-precision floating-point numbers.
	 *
	 * @tparam N Number of elements
	 */
	/// @{
	template <std::size_t N>
	using dvec = math::vector<double, N>;
	using dvec2 = dvec<2>;
	using dvec3 = dvec<3>;
	using dvec4 = dvec<4>;
	/// @}
	
	/// @}
	
	/// @name Matrix types
	/// @{
	
	/**
	 * *n* by *m* matrix of floating-point numbers.
	 *
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
	/// @{
	template <std::size_t N, std::size_t M>
	using fmat = math::matrix<float, N, M>;
	using fmat2x2 = fmat<2, 2>;
	using fmat2x3 = fmat<2, 3>;
	using fmat2x4 = fmat<2, 4>;
	using fmat3x2 = fmat<3, 2>;
	using fmat3x3 = fmat<3, 3>;
	using fmat3x4 = fmat<3, 4>;
	using fmat4x2 = fmat<4, 2>;
	using fmat4x3 = fmat<4, 3>;
	using fmat4x4 = fmat<4, 4>;
	using mat2x2 = fmat2x2;
	using mat2x3 = fmat2x3;
	using mat2x4 = fmat2x4;
	using mat3x2 = fmat3x2;
	using mat3x3 = fmat3x3;
	using mat3x4 = fmat3x4;
	using mat4x2 = fmat4x2;
	using mat4x3 = fmat4x3;
	using mat4x4 = fmat4x4;
	/// @}
	
	/**
	 * *n* by *n* square matrix of floating-point numbers.
	 *
	 * @tparam N Number of columns and rows.
	 */
	/// @{
	using fmat2 = fmat2x2;
	using fmat3 = fmat3x3;
	using fmat4 = fmat4x4;
	using mat2 = fmat2;
	using mat3 = fmat3;
	using mat4 = fmat4;
	/// @}
	
	/**
	 * *n* by *m* matrix of double-precision floating-point numbers.
	 *
	 * @tparam N Number of columns.
	 * @tparam M Number of rows.
	 */
	/// @{
	template <std::size_t N, std::size_t M>
	using dmat = math::matrix<double, N, M>;
	using dmat2x2 = dmat<2, 2>;
	using dmat2x3 = dmat<2, 3>;
	using dmat2x4 = dmat<2, 4>;
	using dmat3x2 = dmat<3, 2>;
	using dmat3x3 = dmat<3, 3>;
	using dmat3x4 = dmat<3, 4>;
	using dmat4x2 = dmat<4, 2>;
	using dmat4x3 = dmat<4, 3>;
	using dmat4x4 = dmat<4, 4>;
	/// @}
	
	/**
	 * *n* by *n* square matrix of double-precision floating-point numbers.
	 *
	 * @tparam N Number of columns and rows.
	 */
	/// @{
	using dmat2 = dmat2x2;
	using dmat3 = dmat3x3;
	using dmat4 = dmat4x4;
	/// @}
	
	/// @}
	
	/// @name Quaternion types
	/// @{
	
	/**
	 * Quaternion with floating-point scalars.
	 *
	 * @tparam T Scalar type.
	 */
	/// @{
	using fquat = math::quaternion<float>;
	using quat = fquat;
	/// @}
	
	/**
	 * Quaternion with double-precision floating-point scalars.
	 *
	 * @tparam T Scalar type.
	 */
	using dquat = math::quaternion<double>;
	
	/// @}

} // namespace types

// Bring GLSL types into glsl namespace
using namespace types;

} // namespace glsl
} // namespace math

#endif // ANTKEEPER_MATH_GLSL_HPP

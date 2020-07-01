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

#ifndef ANTKEEPER_MATH_HPP
#define ANTKEEPER_MATH_HPP

#include <cstdlib>
#include <type_traits>
#include <vmq/vmq.hpp>

using namespace vmq::operators;

inline float frand(float start, float end)
{
    float f = (float)std::rand() / RAND_MAX;
    return start + f * (end - start);
}

namespace math {

/**
 * Reinterprets data as an `N`-dimensional vector of type `T`.
 *
 * @tparam N Number of vector dimensions.
 * @tparam T Scalar type.
 * @param data Data to reinterpret.
 */
template <std::size_t N, typename T>
inline vmq::vector<T, N>& as_vector(T& data)
{
	static_assert(std::is_pod<vmq::vector<T, N>>::value);
	return reinterpret_cast<vmq::vector<T, N>&>(data);
}

/**
 * Reinterprets data as an `NxM` matrix of type `T`.
 *
 * @tparam N Number of columns.
 * @tparam M Number of rows.
 * @tparam T Element type.
 * @param data Data to reinterpret.
 */
template <std::size_t N, std::size_t M, typename T>
inline vmq::matrix<T, N, M>& as_matrix(T& data)
{
	static_assert(std::is_pod<vmq::matrix<T, N, M>>::value);
	return reinterpret_cast<vmq::matrix<T, N, M>&>(data);
}

template <class T>
vmq::vector<T, 3> project_on_plane(const vmq::vector<T, 3>& v, const vmq::vector<T, 3>& p, const vmq::vector<T, 3>& n)
{
	return v - n * vmq::dot(v - p, n);
}

} // namespace math

#endif // ANTKEEPER_MATH_HPP


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

#ifndef ANTKEEPER_MATH_TRANSFORM_FUNCTIONS_HPP
#define ANTKEEPER_MATH_TRANSFORM_FUNCTIONS_HPP

#include <engine/math/transform-type.hpp>
#include <engine/math/quaternion.hpp>

namespace math {

/**
 * Calculates the inverse of a transform.
 *
 * @param t Transform of which to take the inverse.
 */
template <class T>
[[nodiscard]] transform<T> inverse(const transform<T>& t);

/**
 * Converts a transform to a transformation matrix.
 *
 * @param t Transform.
 * @return Matrix representing the transformation described by `t`.
 */
template <class T>
[[nodiscard]] matrix<T, 4, 4> matrix_cast(const transform<T>& t);

/**
 * Multiplies two transforms.
 *
 * @param x First transform.
 * @param y Second transform.
 * @return Product of the two transforms.
 */
template <class T>
[[nodiscard]] transform<T> mul(const transform<T>& x, const transform<T>& y);

/**
 * Multiplies a vector by a transform.
 *
 * @param t Transform.
 * @param v Vector.
 * @return Product of the transform and vector.
 */
template <class T>
[[nodiscard]] vector<T, 3> mul(const transform<T>& t, const vector<T, 3>& v);

template <class T>
transform<T> inverse(const transform<T>& t)
{
	transform<T> inverse_t;
	inverse_t.scale = {T(1) / t.scale.x(), T(1) / t.scale.y(), T(1) / t.scale.z()};
	inverse_t.rotation = conjugate(t.rotation);
	inverse_t.translation = negate(mul(inverse_t.rotation, t.translation));
	return inverse_t;
}

template <class T>
matrix<T, 4, 4> matrix_cast(const transform<T>& t)
{
	matrix<T, 4, 4> transformation = matrix<T, 4, 4>(matrix<T, 3, 3>(t.rotation));
	transformation[3] = {t.translation[0], t.translation[1], t.translation[2], T(1)};
	return scale(transformation, t.scale);
}

template <class T>
transform<T> mul(const transform<T>& x, const transform<T>& y)
{
	return
		{
			mul(x, y.translation),
			normalize(mul(x.rotation, y.rotation)),
			mul(x.scale, y.scale)
		};
}

template <class T>
vector<T, 3> mul(const transform<T>& t, const vector<T, 3>& v)
{
	return add(t.translation, (mul(t.rotation, mul(v, t.scale))));
}

} // namespace math

#endif // ANTKEEPER_MATH_TRANSFORM_FUNCTIONS_HPP


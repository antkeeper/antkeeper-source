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

#ifndef ANTKEEPER_RAY_HPP
#define ANTKEEPER_RAY_HPP

#include <vmq/vmq.hpp>

using vmq::vector;
using namespace vmq::operators;

template <class T>
struct ray
{
	vector<T, 3> origin;
	vector<T, 3> direction;

	vector<T, 3> extrapolate(T distance) const;
};

template <class T>
inline vector<T, 3> ray<T>::extrapolate(T distance) const
{
	return origin + direction * distance;
}

#endif // ANTKEEPER_RAY_HPP


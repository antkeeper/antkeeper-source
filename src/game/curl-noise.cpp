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

#include "curl-noise.hpp"

inline static Vector3 gradient(const Vector3& p, float frequency)
{
	const float epsilon = 0.0001f;

	float n0 = glm::perlin(p * frequency);
	float nx = glm::perlin((p + Vector3(epsilon, 0, 0)) * frequency);
	float ny = glm::perlin((p + Vector3(0, epsilon, 0)) * frequency);
	float nz = glm::perlin((p + Vector3(0, 0, epsilon)) * frequency);

	return Vector3(nx - n0, ny - n0, nz - n0) * (1.0f / epsilon);
}

Vector3 curl(const Vector3& p, const Vector3& offset, float frequency)
{
	Vector3 g1 = gradient(p, frequency);
	Vector3 g2 = gradient(p + offset, frequency);
	return glm::cross(g1, g2);
}


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

#include "intersection.hpp"
#include <limits>

namespace geom {

std::tuple<bool, float> ray_plane_intersection(const ray<float>& ray, const plane<float>& plane)
{
	float denom = math::dot(ray.direction, plane.normal);
	if (denom != 0.0f)
	{
		float t = -(math::dot(ray.origin, plane.normal) + plane.distance) / denom;
		
		if (t >= 0.0f)
		{
			return std::make_tuple(true, t);
		}
	}
	
	return std::make_tuple(false, std::numeric_limits<float>::infinity());
}

std::tuple<bool, float, float, float> ray_triangle_intersection(const ray<float>& ray, const float3& a, const float3& b, const float3& c)
{
	// Find edges
	float3 edge10 = b - a;
	float3 edge20 = c - a;

	// Calculate determinant
	float3 pv = math::cross(ray.direction, edge20);
	float det = math::dot(edge10, pv);

	if (!det)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
	}

	float inverse_det = 1.0f / det;

	// Calculate u
	float3 tv = ray.origin - a;
	float u = math::dot(tv, pv) * inverse_det;
	
	if (u < 0.0f || u > 1.0f)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
	}

	// Calculate v
	float3 qv = math::cross(tv, edge10);
	float v = math::dot(ray.direction, qv) * inverse_det;

	if (v < 0.0f || u + v > 1.0f)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
	}

	// Calculate t
	float t = math::dot(edge20, qv) * inverse_det;

	if (t > 0.0f)
	{
		return std::make_tuple(true, t, u, v);
	}

	return std::make_tuple(false, std::numeric_limits<float>::infinity(), 0.0f, 0.0f);
}

std::tuple<bool, float, float> ray_aabb_intersection(const ray<float>& ray, const aabb<float>& aabb)
{
	float t0 = -std::numeric_limits<float>::infinity();
	float t1 = std::numeric_limits<float>::infinity();
	
	for (std::size_t i = 0; i < 3; ++i)
	{
		if (ray.direction[i] == 0.0f)
		{
			if (ray.origin[i] < aabb.min_point[i] || ray.origin[i] > aabb.max_point[i])
			{
				return std::make_tuple(false, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());	
			}
		}
		else
		{
			float tmin = (aabb.min_point[i] - ray.origin[i]) / ray.direction[i];
			float tmax = (aabb.max_point[i] - ray.origin[i]) / ray.direction[i];
			
			t0 = std::max(t0, std::min(tmin, tmax));
			t1 = std::min(t1, std::max(tmin, tmax));
		}
	}
	
	if (t0 > t1 || t1 < 0.0f)
	{
		return std::make_tuple(false, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	}
	
	return std::make_tuple(true, t0, t1);
}

std::tuple<bool, float, float, std::size_t, std::size_t> ray_mesh_intersection(const ray<float>& ray, const mesh& mesh)
{
	const std::vector<mesh::face*>& triangles = mesh.get_faces();
	
	bool intersection = false;
	float t0 = std::numeric_limits<float>::infinity();
	float t1 = -std::numeric_limits<float>::infinity();
	std::size_t index0 = triangles.size();
	std::size_t index1 = triangles.size();

	for (std::size_t i = 0; i < triangles.size(); ++i)
	{
		const mesh::face* triangle = triangles[i];

		const float3& a = reinterpret_cast<const float3&>(triangle->edge->vertex->position);
		const float3& b = reinterpret_cast<const float3&>(triangle->edge->next->vertex->position);
		const float3& c = reinterpret_cast<const float3&>(triangle->edge->previous->vertex->position);

		auto result = ray_triangle_intersection(ray, a, b, c);

		if (std::get<0>(result))
		{
			intersection = true;
			float t = std::get<1>(result);

			if (t < t0)
			{
				t0 = t;
				index0 = i;
			}

			if (t > t1)
			{
				t1 = t;
				index1 = i;
			}
		}
	}

	return std::make_tuple(intersection, t0, t1, index0, index1);
}

bool aabb_aabb_intersection(const aabb<float>& a, const aabb<float>& b)
{
	if (a.max_point.x < b.min_point.x || a.min_point.x > b.max_point.x)
		return false;
	if (a.max_point.y < b.min_point.y || a.min_point.y > b.max_point.y)
		return false;
	if (a.max_point.z < b.min_point.z || a.min_point.z > b.max_point.z)
		return false;
	return true;
}

bool aabb_sphere_intersection(const aabb<float>& aabb, const float3& center, float radius)
{
	float distance_squared = 0.0f;
	for (int i = 0; i < 3; ++i)
	{
		float v = center[i];
		if (v < aabb.min_point[i])
			distance_squared += (aabb.min_point[i] - v) * (aabb.min_point[i] - v);
		if (v > aabb.max_point[i])
			distance_squared += (v - aabb.max_point[i]) * (v - aabb.max_point[i]);
	}

	return (distance_squared <= (radius * radius));
}

} // namespace geom

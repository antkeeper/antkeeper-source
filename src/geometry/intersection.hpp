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

#ifndef ANTKEEPER_INTERSECTION_HPP
#define ANTKEEPER_INTERSECTION_HPP

#include <tuple>
#include <vmq/vmq.hpp>
#include "aabb.hpp"
#include "mesh.hpp"
#include "plane.hpp"
#include "ray.hpp"

using namespace vmq::types;
using namespace vmq::operators;


/**
 * Tests for intersection between a ray and a plane.
 *
 * @param ray Ray to test for intersection.
 * @param plane Plane to test for intersection.
 * @return Tuple containing a boolean indicating whether intersection occurred, and a float indicating the signed distance along the ray to the point of intersection.
 */
std::tuple<bool, float> ray_plane_intersection(const ray<float>& ray, const plane<float>& plane);

std::tuple<bool, float, float, float> ray_triangle_intersection(const ray<float>& ray, const float3& a, const float3& b, const float3& c);

std::tuple<bool, float, float> ray_aabb_intersection(const ray<float>& ray, const aabb<float>& aabb);

std::tuple<bool, float, float, std::size_t, std::size_t> ray_mesh_intersection(const ray<float>& ray, const mesh& mesh);

bool aabb_sphere_intersection(const aabb<float>& aabb, const float3& center, float radius);

bool aabb_aabb_intersection(const aabb<float>& a, const aabb<float>& b);

#endif // ANTKEEPER_INTERSECTION_HPP


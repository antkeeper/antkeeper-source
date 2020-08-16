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

#ifndef ANTKEEPER_SUBTERRAIN_SYSTEM_HPP
#define ANTKEEPER_SUBTERRAIN_SYSTEM_HPP

#include "entity-system.hpp"
#include "geometry/mesh.hpp"
#include "geometry/aabb.hpp"
#include "utility/fundamental-types.hpp"
#include <unordered_map>

class resource_manager;
class model;
class model_group;
class material;
struct cube_tree;
class scene;
class model_instance;

template <std::int64_t Mantissa, std::int64_t Exponent>
struct epsilon
{
	static const double value;
};

template <std::int64_t Mantissa, std::int64_t Exponent>
const double epsilon<Mantissa, Exponent>::value = static_cast<double>(Mantissa) * std::pow(10.0, Exponent);

typedef epsilon<1, -5> epsilon_1en5;

template <class Epsilon, class T, std::size_t N>
struct vector_hasher
{
	typedef math::vector<T, N> vector_type;

	std::size_t operator()(const vector_type& v) const noexcept
	{
		static const T inverse_epsilon = T(1) / Epsilon::value;

		std::size_t hash = 0;
		for (std::size_t i = 0; i < N; ++i)
		{
			std::int64_t j = static_cast<std::int64_t>(v[i] * inverse_epsilon);
			hash ^= std::hash<std::int64_t>()(j) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}

		return hash;
	}
};

template <class Epsilon, class T, std::size_t N>
struct vector_equals
{
	typedef math::vector<T, N> vector_type;

	bool operator()(const vector_type& a, const vector_type& b) const noexcept
	{
		for (std::size_t i = 0; i < N; ++i)
		{
			if (std::fabs(b[i] - a[i]) >= Epsilon::value)
				return false;
		}

		return true;
	}
};

class subterrain_system: public entity_system
{
public:
	subterrain_system(entt::registry& registry, ::resource_manager* resource_manager);
	~subterrain_system();
	virtual void update(double t, double dt);
	
	void set_scene(::scene* scene);

private:
	void regenerate_subterrain_mesh();
	void march(::cube_tree* node);
	void regenerate_subterrain_model();
	void dig(const float3&position, float radius);
	float distance(const ::cube_tree& node, const float3& sample) const;

	resource_manager* resource_manager;
	mesh* subterrain_mesh;
	model* subterrain_model;
	material* subterrain_inside_material;
	material* subterrain_outside_material;
	model_group* subterrain_inside_group;
	model_group* subterrain_outside_group;
	int subterrain_model_vertex_size;
	int subterrain_model_vertex_stride;
	aabb<float> subterrain_bounds;
	::cube_tree* cube_tree;
	std::vector<float3> subterrain_vertices;
	std::vector<std::array<std::uint_fast32_t, 3>> subterrain_triangles;
	float isosurface_resolution;
	bool first_run;
	int merged;

	std::unordered_map<
		float3,
		std::uint_fast32_t,
		vector_hasher<epsilon_1en5, float, 3>,
		vector_equals<epsilon_1en5, float, 3>> subterrain_vertex_map;
	
	::scene* scene;
	model_instance* subterrain_model_instance;
};

#endif // ANTKEEPER_SUBTERRAIN_SYSTEM_HPP


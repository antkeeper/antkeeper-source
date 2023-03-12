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

#ifndef ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP
#define ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include "game/components/terrain-component.hpp"
#include <engine/entity/id.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/geom/quadtree.hpp>
#include <engine/geom/mesh.hpp>
#include <engine/utility/fundamental-types.hpp>
#include <engine/render/model.hpp>
#include <engine/render/material.hpp>
#include <engine/scene/static-mesh.hpp>
#include <engine/scene/collection.hpp>
#include <engine/geom/view-frustum.hpp>
#include <unordered_map>
#include <stack>


/**
 * Generates terrain patches and performs terrain patch LOD selection.
 */
class terrain_system: public updatable_system
{
public:
	explicit terrain_system(entity::registry& registry);
	~terrain_system();
	
	virtual void update(float t, float dt);
	
	/**
	 * Sets the size of a patch.
	 *
	 * @param length Side length of a patch.
	 */
	void set_patch_side_length(float length);
	
	/**
	 * Sets the number of subdivisions of a patch. Zero subdivisions results in a single quad, one subdivison results in four quads, etc.
	 *
	 * @param n Number of subdivisions.
	 */
	void set_patch_subdivisions(std::size_t n);
	
	/**
	 * Sets the material of each patch.
	 *
	 * @param material Patch material.
	 */
	void set_patch_material(std::shared_ptr<::render::material> material);
	
	/**
	 * Sets the terrain elevation function.
	 *
	 * @param f Function which returns the terrain height (Y-coordinate) given X- and Z-coordinates.
	 */
	void set_elevation_function(const std::function<float(float, float)>& f);
	
	/**
	 * Sets the scene collection into which terrain patch model instances will be inserted.
	 */
	void set_scene_collection(scene::collection* collection);

private:
	typedef geom::unordered_quadtree16 quadtree_type;
	typedef typename quadtree_type::node_type quadtree_node_type;
	
	void balance_quadtree();
	
	struct patch
	{
		geom::mesh* mesh;
		::render::model* model;
		scene::static_mesh* static_mesh;
	};
	
	void on_terrain_construct(entity::registry& registry, entity::id entity_id);
	void on_terrain_update(entity::registry& registry, entity::id entity_id);
	void on_terrain_destroy(entity::registry& registry, entity::id entity_id);
	
	float get_patch_size(quadtree_node_type node) const;
	float3 get_patch_center(quadtree_node_type node) const;
	
	void rebuild_patch_base_mesh();
	
	/**
	 * Generates a mesh for a terrain patch given the patch's quadtree node
	 */
	std::unique_ptr<geom::mesh> generate_patch_mesh(quadtree_node_type node) const;
	
	/**
	 * Generates a model for a terrain patch given the patch's mesh.
	 */
	::render::model* generate_patch_model(quadtree_node_type node) const;
	
	patch* generate_patch(quadtree_node_type node);
	
	float patch_side_length;
	std::size_t patch_subdivisions;
	std::size_t patch_cell_count;
	std::size_t patch_triangle_count;
	std::size_t patch_vertex_size;
	std::size_t patch_vertex_stride;
	mutable std::vector<float> patch_vertex_data;
	
	struct patch_vertex
	{
		float3 position;
		float2 uv;
		float3 normal;
		float3 tangent;
		float3 bitangent;
		float bitangent_sign;
	};
	
	mutable std::vector<std::vector<patch_vertex>> patch_vertex_buffer;

	
	std::shared_ptr<::render::material> patch_material;
	std::function<float(float, float)> elevation_function;
	scene::collection* scene_collection;
	
	std::unique_ptr<geom::mesh> patch_base_mesh;
	
	/// Quadtree describing level of detail
	quadtree_type quadtree;
	float quadtree_node_size[quadtree_type::max_depth + 1];
	quadtree_node_type quadtree_node_resolution[quadtree_type::max_depth + 1];
	
	/// Map linking quadtree nodes to terrain patches
	std::unordered_map<quadtree_node_type, patch*> patches;
	
	std::stack<quadtree_node_type> node_stack;
};


#endif // ANTKEEPER_GAME_TERRAIN_SYSTEM_HPP

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

#ifndef ANTKEEPER_GAME_SYSTEM_TERRAIN_HPP
#define ANTKEEPER_GAME_SYSTEM_TERRAIN_HPP

#include "game/system/updatable.hpp"
#include "game/component/terrain.hpp"
#include "entity/id.hpp"
#include "math/quaternion-type.hpp"
#include "geom/quadtree.hpp"
#include "geom/mesh.hpp"
#include "utility/fundamental-types.hpp"
#include "render/model.hpp"
#include "render/material.hpp"
#include "scene/model-instance.hpp"
#include "scene/collection.hpp"
#include <unordered_map>

namespace game {
namespace system {

/**
 * Generates and manages terrain with LOD based on distance to observers.
 */
class terrain: public updatable
{
public:
	terrain(entity::registry& registry);
	~terrain();
	
	virtual void update(double t, double dt);
	
	/**
	 * Sets the number of subdivisions for a patch. Zero subdivisions results in a single quad, one subdivison results in four quads, etc.
	 *
	 * @param n Number of subdivisions.
	 */
	void set_patch_subdivisions(std::uint8_t n);
	
	/**
	 * Sets the scene collection into which terrain patch model instances will be inserted.
	 */
	void set_patch_scene_collection(scene::collection* collection);
	
	/**
	 * Sets the maximum tolerable screen-space error.
	 *
	 * If the screen-space error of a terrain patch exceeds the maximum tolerable value, it will be subdivided.
	 *
	 * @param error Maximum tolerable screen-space error.
	 */
	void set_max_error(double error);

private:
	typedef geom::quadtree64 quadtree_type;
	typedef quadtree_type::node_type quadtree_node_type;
	
	struct terrain_patch
	{
		geom::mesh* mesh;
		::render::model* model;
		scene::model_instance* model_instance;
		float error;
		float morph;
	};
	
	/// Single face of a terrain quadsphere
	struct terrain_quadsphere_face
	{
		/// Quadtree describing level of detail
		quadtree_type quadtree;
		
		/// Map linking quadtree nodes to terrain patches
		std::unordered_map<quadtree_node_type, terrain_patch*> patches;
	};
	
	/// A terrain quadsphere with six faces.
	struct terrain_quadsphere
	{
		/// Array of six terrain quadsphere faces, in the order of +x, -x, +y, -y, +z, -z.
		terrain_quadsphere_face faces[6];
	};
	
	
	static double screen_space_error(double horizontal_fov, double horizontal_resolution, double distance, double geometric_error);
	
	void on_terrain_construct(entity::registry& registry, entity::id entity_id);
	void on_terrain_destroy(entity::registry& registry, entity::id entity_id);
	
	/**
	 * Generates a mesh for a terrain patch given the patch's quadtree node
	 */
	geom::mesh* generate_patch_mesh(std::uint8_t face_index, quadtree_node_type node, double body_radius, const std::function<double(double, double)>& elevation) const;
	
	/**
	 * Generates a model for a terrain patch given the patch's mesh.
	 */
	::render::model* generate_patch_model(const geom::mesh& patch_mesh, ::render::material* patch_material) const;
	
	
	
	/// @TODO horizon culling
	
	std::uint8_t patch_subdivisions;
	std::size_t patch_vertex_size;
	std::size_t patch_vertex_stride;
	std::size_t patch_vertex_count;
	float* patch_vertex_data;
	math::quaternion<double> face_rotations[6];
	geom::mesh* patch_base_mesh;
	scene::collection* patch_scene_collection;
	double max_error;
	
	std::unordered_map<entity::id, terrain_quadsphere*> terrain_quadspheres;
};

} // namespace system
} // namespace game

#endif // ANTKEEPER_GAME_SYSTEM_TERRAIN_HPP

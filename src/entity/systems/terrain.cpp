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

#include "entity/systems/terrain.hpp"
#include "utility/fundamental-types.hpp"
#include "entity/components/observer.hpp"
#include "entity/components/terrain.hpp"
#include "entity/components/celestial-body.hpp"
#include "geom/quadtree.hpp"


/**
 * A cube with six quadtrees as faces.
 */
struct quadtree_cube
{
	typedef geom::quadtree32 quadtree_type;
	typedef quadtree_type::node_type node_type;
	
	void clear();
	
	/**
	 * Refines the quadtree cube.
	 *
	 * @param threshold Function object which, given a quadsphere face index and quadtree node, returns `true` if the node should be subdivided, and `false` otherwise.
	 */
	void refine(const std::function<bool(std::uint8_t, node_type)>& threshold);
	
	quadtree_type faces[6];
};

void quadtree_cube::clear()
{
	for (std::uint8_t i = 0; i < 6; ++i)
		faces[i].clear();
}

void quadtree_cube::refine(const std::function<bool(std::uint8_t, node_type)>& threshold)
{
	for (std::uint8_t i = 0; i < 6; ++i)
	{
		for (auto it = faces[i].begin(); it != faces[i].end(); ++it)
		{
			node_type node = *it;
			
			if (threshold(i, node))
				faces[i].insert(quadtree_type::child(node, 0));
		}
	}
}

/*
terrain_qtc.refine
(
	[observer](std::uint8_t face_index, quadtree_cube_type::node_type node) -> bool
	{	
		// Extract morton location code
		quadtree_type::node_type location = quadtree_type::location(node);
		quadtree_type::node_type morton_x;
		quadtree_type::node_type morton_y;
		geom::morton::decode(location, morton_x, morton_y);
		
		// Extract depth
		quadtree_type::node_type depth = quadtree_type::depth(node);
		
		// Determine fractional side length at depth
		float length = 1.0f / std::exp2(depth);
		
		// Determine fractional center of node
		float3 center;
		center.x = (static_cast<float>(morton_x) * length + length * 0.5f) * 2.0f - 1.0f;
		center.y = (static_cast<float>(morton_y) * length + length * 0.5f) * 2.0f - 1.0f;
		center.z = 1.0f;
		
		// Project node center onto unit sphere
		center = math::normalize(center);
		
		// Rotate projected center into sphere space
		center = face_rotations[face_index] * center;
		
		// Scale center by body radius
		center *= body_radius;
		
		// Calculate distance from observer to node center
		float distance = math::length(projected_center - observer_location);
		
		if (depth < 4 && distance < ...)
			return true;
		
		return false;
	}
);
*/

/**
 * Queries a quad sphere for a list of leaf nodes. Leaf nodes will be inserted in the set
 *
 *
 * 0. If observer position changed more than x amount:
 * 1. Clear quad sphere
 * 2. Insert leaves based on observer distance.
 * 3. Pass quad sphere to tile generation function.
 * 3. Iterate leaves, deriving the face, depth, and morton location from each leaf index.
 * 4. Face, depth, and morton location can be used to determine latitude, longitude, and generate tiles.
 * 5. Generated tiles cached and added to scene.
 * 6. Record position of observer
 */

/**
 * Lat, lon determination:
 *
 * 1. Use morton location and depth to determine the x-y coordinates on a planar cube face.
 * 2. Project x-y coordinates onto sphere.
 * 3. Rotate coordinates according to face index.
 * 4. Convert cartesian coordinates to spherical coordinates.
 */


namespace entity {
namespace system {

terrain::terrain(entity::registry& registry):
	updatable(registry),
	patch_subdivisions(0),
	patch_vertex_size(0),
	patch_vertex_count(0),
	patch_vertex_data(nullptr)
{
	// position + uv + normal + tangent + barycentric
	patch_vertex_size = 3 + 2 + 3 + 4 + 3;
	
	
	set_patch_subdivisions(0);
	
	registry.on_construct<component::terrain>().connect<&terrain::on_terrain_construct>(this);
	registry.on_destroy<component::terrain>().connect<&terrain::on_terrain_destroy>(this);
}

terrain::~terrain()
{}

void terrain::update(double t, double dt)
{
	// Subdivide or collapse quad sphere
	registry.view<component::observer>().each(
	[&](entity::id observer_eid, const auto& observer)
	{
		// Skip observers with null reference body
		if (observer.reference_body_eid == entt::null)
			return;
		
		// Skip observers with non-body or non-terrestrial reference bodies
		if (!registry.has<component::celestial_body>(observer.reference_body_eid) ||
			!registry.has<component::terrain>(observer.reference_body_eid))
			return;
		
		const auto& celestial_body = registry.get<component::celestial_body>(observer.reference_body_eid);
		const auto& terrain = registry.get<component::terrain>(observer.reference_body_eid);
		
		// Haversine distance to all 6 faces, then recursively to children
	});
}

void terrain::set_patch_subdivisions(std::uint8_t n)
{
	patch_subdivisions = n;
	
	// Recalculate number of vertices per patch
	patch_vertex_count = static_cast<std::size_t>(std::pow(std::exp2(patch_subdivisions) + 1, 2));
	
	// Resize patch vertex data buffer
	delete[] patch_vertex_data;
	patch_vertex_data = new float[patch_vertex_count * patch_vertex_size];
}

void terrain::on_terrain_construct(entity::registry& registry, entity::id entity_id, component::terrain& component)
{
	// Build quad sphere
}

void terrain::on_terrain_destroy(entity::registry& registry, entity::id entity_id)
{
	// Destroy quad sphere
}

void terrain::generate_patch()
{


}

} // namespace system
} // namespace entity

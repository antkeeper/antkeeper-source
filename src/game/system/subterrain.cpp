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

#include "subterrain.hpp"
#include "game/component/model.hpp"
#include "game/component/cavity.hpp"
#include "entity/id.hpp"
#include "render/model.hpp"
#include "render/material.hpp"
#include "geom/mesh-functions.hpp"
#include "render/vertex-attribute.hpp"
#include "gl/vertex-attribute.hpp"
#include "gl/drawing-mode.hpp"
#include "gl/vertex-buffer.hpp"
#include "resources/resource-manager.hpp"
#include "geom/marching-cubes.hpp"
#include "geom/intersection.hpp"
#include "utility/fundamental-types.hpp"
#include <array>
#include <limits>

namespace game {
namespace system {

/**
 * An octree containing cubes for the marching cubes algorithm.
 */
struct cube_tree
{
public:
	cube_tree(const geom::aabb<float>& bounds, int max_depth);
	~cube_tree();

	const bool is_leaf() const;
	const geom::aabb<float>& get_bounds() const;

	/// Subdivides all nodes intersecting with a region to the max depth.
	void subdivide_max(const geom::aabb<float>& region);

	/// Fills a list with all leaf nodes that intersect with a region.
	void query_leaves(std::list<cube_tree*>& nodes, const geom::aabb<float>& region);
	void visit_leaves(const geom::aabb<float>& region, const std::function<void(cube_tree&)>& f);

	/// Counts then number of nodes in the octree.
	std::size_t size() const;

	cube_tree* children[8];
	float3 corners[8];
	float distances[8];
	const int max_depth;
	const int depth;
	const geom::aabb<float> bounds;

private:
	cube_tree(const geom::aabb<float>& bounds, int max_depth, int depth);
	void subdivide();
};

cube_tree::cube_tree(const geom::aabb<float>& bounds, int max_depth):
	cube_tree(bounds, max_depth, 0)
{}

cube_tree::cube_tree(const geom::aabb<float>& bounds, int max_depth, int depth):
	bounds(bounds),
	max_depth(max_depth),
	depth(depth)
{
	corners[0] = {bounds.min_point.x(), bounds.min_point.y(), bounds.min_point.z()};
	corners[1] = {bounds.max_point.x(), bounds.min_point.y(), bounds.min_point.z()};
	corners[2] = {bounds.max_point.x(), bounds.max_point.y(), bounds.min_point.z()};
	corners[3] = {bounds.min_point.x(), bounds.max_point.y(), bounds.min_point.z()};
	corners[4] = {bounds.min_point.x(), bounds.min_point.y(), bounds.max_point.z()};
	corners[5] = {bounds.max_point.x(), bounds.min_point.y(), bounds.max_point.z()};
	corners[6] = {bounds.max_point.x(), bounds.max_point.y(), bounds.max_point.z()};
	corners[7] = {bounds.min_point.x(), bounds.max_point.y(), bounds.max_point.z()};

	for (int i = 0; i < 8; ++i)
	{
		children[i] = nullptr;
		distances[i] = -std::numeric_limits<float>::infinity();

		// For outside normals
		//distances[i] = std::numeric_limits<float>::infinity();
	}
}

cube_tree::~cube_tree()
{
	for (cube_tree* child: children)
		delete child;
}

void cube_tree::subdivide_max(const geom::aabb<float>& region)
{
	if (depth != max_depth && aabb_aabb_intersection(bounds, region))
	{
		if (is_leaf())
			subdivide();

		for (cube_tree* child: children)
			child->subdivide_max(region);
	}
}

void cube_tree::query_leaves(std::list<cube_tree*>& nodes, const geom::aabb<float>& region)
{
	if (aabb_aabb_intersection(bounds, region))
	{
		if (is_leaf())
		{
			nodes.push_back(this);
		}
		else
		{
			for (cube_tree* child: children)
				child->query_leaves(nodes, region);
		}
	}
}

void cube_tree::visit_leaves(const geom::aabb<float>& region, const std::function<void(cube_tree&)>& f)
{
	if (aabb_aabb_intersection(bounds, region))
	{
		if (is_leaf())
		{
			f(*this);
		}
		else
		{
			for (cube_tree* child: children)
				child->visit_leaves(region, f);
		}
	}
}

std::size_t cube_tree::size() const
{
	std::size_t node_count = 1;
	if (!is_leaf())
	{
		for (cube_tree* child: children)
			node_count += child->size();
	}

	return node_count;
}

inline const bool cube_tree::is_leaf() const
{
	return (children[0] == nullptr);
}

inline const geom::aabb<float>& cube_tree::get_bounds() const
{
	return bounds;
}

void cube_tree::subdivide()
{
	const float3 center = (bounds.min_point + bounds.max_point) * 0.5f;

	for (int i = 0; i < 8; ++i)
	{
		geom::aabb<float> child_bounds;
		for (int j = 0; j < 3; ++j)
		{
			child_bounds.min_point[j] = std::min<float>(corners[i][j], center[j]);
			child_bounds.max_point[j] = std::max<float>(corners[i][j], center[j]);
		}

		children[i] = new cube_tree(child_bounds, max_depth, depth + 1);
	}
}

subterrain::subterrain(entity::registry& registry, ::resource_manager* resource_manager):
	updatable(registry),
	resource_manager(resource_manager)
{

	// Load subterrain materials
	subterrain_inside_material = nullptr;//resource_manager->load<::render::material>("subterrain-inside.mtl");
	subterrain_outside_material = nullptr;//resource_manager->load<::render::material>("subterrain-outside.mtl");

	// Allocate subterrain model
	subterrain_model = new ::render::model();

	// Create inside model group
	subterrain_inside_group = subterrain_model->add_group("inside");
	subterrain_inside_group->set_material(subterrain_inside_material);
	subterrain_inside_group->set_drawing_mode(gl::drawing_mode::triangles);
	subterrain_inside_group->set_start_index(0);
	subterrain_inside_group->set_index_count(0);

	// Create outside model group
	subterrain_outside_group = subterrain_model->add_group("outside");
	subterrain_outside_group->set_material(subterrain_outside_material);
	subterrain_outside_group->set_drawing_mode(gl::drawing_mode::triangles);
	subterrain_outside_group->set_start_index(0);
	subterrain_outside_group->set_index_count(0);

	// Determine vertex size (position, normal, barycentric)
	subterrain_model_vertex_size = 3 + 3 + 3;
	subterrain_model_vertex_stride = subterrain_model_vertex_size * sizeof(float);
	
	// Get model VBO and VAO
	gl::vertex_buffer* vbo = subterrain_model->get_vertex_buffer();
	gl::vertex_array* vao = subterrain_model->get_vertex_array();
	
	std::size_t attribute_offset = 0;
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo;
	position_attribute.offset = attribute_offset;
	position_attribute.stride = subterrain_model_vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define normal vertex attribute
	gl::vertex_attribute normal_attribute;
	normal_attribute.buffer = vbo;
	normal_attribute.offset = attribute_offset;
	normal_attribute.stride = subterrain_model_vertex_stride;
	normal_attribute.type = gl::vertex_attribute_type::float_32;
	normal_attribute.components = 3;
	attribute_offset += normal_attribute.components * sizeof(float);
	
	// Define barycentric vertex attribute
	gl::vertex_attribute barycentric_attribute;
	barycentric_attribute.buffer = vbo;
	barycentric_attribute.offset = attribute_offset;
	barycentric_attribute.stride = subterrain_model_vertex_stride;
	barycentric_attribute.type = gl::vertex_attribute_type::float_32;
	barycentric_attribute.components = 3;
	attribute_offset += barycentric_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	vao->bind(::render::vertex_attribute::position, position_attribute);
	vao->bind(::render::vertex_attribute::normal, normal_attribute);
	vao->bind(::render::vertex_attribute::barycentric, barycentric_attribute);

	// Calculate adjusted bounds to fit isosurface resolution
	//isosurface_resolution = 0.325f;
	isosurface_resolution = 0.5f;
	float ideal_volume_size = 200.0f;
	int octree_depth = std::floor(std::log(ideal_volume_size / isosurface_resolution) / std::log(2)) + 1;
	float adjusted_volume_size = std::pow(2.0f, octree_depth) * isosurface_resolution;

	// Set subterrain bounds
	subterrain_bounds.min_point = float3{-0.5f, -1.0f, -0.5f} * adjusted_volume_size;
	subterrain_bounds.max_point = float3{ 0.5f,  0.0f,  0.5f} * adjusted_volume_size;
	
	// Set subterrain model bounds
	subterrain_model->set_bounds(subterrain_bounds);

	// Allocate cube tree
	cube_tree = new game::system::cube_tree(subterrain_bounds, octree_depth);

	// Allocate mesh
	subterrain_mesh = new geom::mesh();

	first_run = true;
}

subterrain::~subterrain()
{
	delete subterrain_model;
	delete subterrain_mesh;
}

void subterrain::update(double t, double dt)
{
	if (first_run)
	{
		first_run = false;
		//auto subterrain_entity = registry.create();
		//registry.assign<component::model>(subterrain_entity, subterrain_model);
		
		subterrain_model_instance = new scene::model_instance(subterrain_model);
		collection->add_object(subterrain_model_instance);
	}

	bool digging = false;

	registry.view<component::cavity>().each(
		[this, &digging](entity::id entity_id, auto& cavity)
		{
			this->dig(cavity.position, cavity.radius);
			this->registry.destroy(entity_id);

			digging = true;
		});
	
	if (digging)
	{
		
		//std::cout << "regenerating subterrain mesh...\n";
		regenerate_subterrain_mesh();
		//std::cout << "regenerating subterrain mesh... done\n";

		//std::cout << "regenerating subterrain model...\n";
		regenerate_subterrain_model();
		//std::cout << "regenerating subterrain model... done\n";
	}
}

void subterrain::set_scene(scene::collection* collection)
{
	this->collection = collection;
}

void subterrain::regenerate_subterrain_mesh()
{
	delete subterrain_mesh;
	subterrain_mesh = new geom::mesh();
	subterrain_vertices.clear();
	subterrain_triangles.clear();
	subterrain_vertex_map.clear();

	//std::cout << "marching...\n";
	merged = 0;
	march(cube_tree);
	//std::cout << "merged " << merged << " vertices\n";
	//std::cout << "marching...done\n";

	//std::cout << "vertex count: " << subterrain_vertices.size() << std::endl;
	//std::cout << "triangle count: " << subterrain_triangles.size() << std::endl;

	//std::cout << "creating mesh...\n";
	create_triangle_mesh(*subterrain_mesh, subterrain_vertices, subterrain_triangles);
	//std::cout << "creating mesh... done\n";
}

void subterrain::march(game::system::cube_tree* node)
{
	if (!node->is_leaf())
	{
		for (game::system::cube_tree* child: node->children)
			march(child);
		return;
	}
	else if (node->depth != node->max_depth)
	{
		return;
	}

	// Get node bounds
	const geom::aabb<float>& bounds = node->get_bounds();

	// Polygonize cube
	float vertex_buffer[12 * 3];
	std::uint_fast8_t vertex_count;
	std::int_fast8_t triangle_buffer[5 * 3];
	std::uint_fast8_t triangle_count;
	const float* corners = &node->corners[0][0];
	const float* distances = &node->distances[0];
	geom::mc::polygonize(vertex_buffer, &vertex_count, triangle_buffer, &triangle_count, corners, distances);

	// Remap local vertex buffer indices (0-11) to mesh vertex indices
	std::uint_fast32_t vertex_remap[12];
	for (int i = 0; i < vertex_count; ++i)
	{
		const float3& vertex = reinterpret_cast<const float3&>(vertex_buffer[i * 3]);

		if (auto it = subterrain_vertex_map.find(vertex); it != subterrain_vertex_map.end())
		{
			vertex_remap[i] = it->second;
			++merged;
		}
		else
		{
			vertex_remap[i] = subterrain_vertices.size();
			subterrain_vertex_map[vertex] = subterrain_vertices.size();
			subterrain_vertices.push_back(vertex);
		}
	}

	// Add triangles
	for (std::uint_fast32_t i = 0; i < triangle_count; ++i)
	{
		subterrain_triangles.push_back(
			{
				vertex_remap[triangle_buffer[i * 3]],
				vertex_remap[triangle_buffer[i * 3 + 1]],
				vertex_remap[triangle_buffer[i * 3 + 2]]
			});
	}
}

void subterrain::regenerate_subterrain_model()
{
	float3* face_normals = new float3[subterrain_mesh->get_faces().size()];
	calculate_face_normals(face_normals, *subterrain_mesh);

	static const float3 barycentric_coords[3] =
	{
		float3{1, 0, 0},
		float3{0, 1, 0},
		float3{0, 0, 1}
	};

	float* vertex_data = new float[subterrain_model_vertex_size * subterrain_mesh->get_faces().size() * 3];
	float* v = vertex_data;
	for (std::size_t i = 0; i < subterrain_mesh->get_faces().size(); ++i)
	{
		geom::mesh::face* face = subterrain_mesh->get_faces()[i];
		geom::mesh::edge* ab = face->edge;
		geom::mesh::edge* bc = face->edge->next;
		geom::mesh::edge* ca = face->edge->previous;
		geom::mesh::vertex* a = ab->vertex;
		geom::mesh::vertex* b = bc->vertex;
		geom::mesh::vertex* c = ca->vertex;
		geom::mesh::vertex* vertices[3] = {a, b, c};

		for (std::size_t j = 0; j < 3; ++j)
		{
			geom::mesh::vertex* vertex = vertices[j];

			float3 n = {0, 0, 0};
			geom::mesh::edge* start = vertex->edge;
			geom::mesh::edge* edge = start;
			do
			{
				if (edge->face)
				{
					n += face_normals[edge->face->index];
				}

				edge = edge->previous->symmetric;
			}
			while (edge != start);
			n = math::normalize(n);

			//float3 n = reinterpret_cast<const float3&>(face_normals[i * 3]);

			*(v++) = vertex->position[0];
			*(v++) = vertex->position[1];
			*(v++) = vertex->position[2];

			*(v++) = n[0];
			*(v++) = n[1];
			*(v++) = n[2];

			*(v++) = barycentric_coords[j][0];
			*(v++) = barycentric_coords[j][1];
			*(v++) = barycentric_coords[j][2];
		}
	}

	// Resized VBO and upload vertex data
	gl::vertex_buffer* vbo = subterrain_model->get_vertex_buffer();
	vbo->resize(subterrain_mesh->get_faces().size() * 3 * subterrain_model_vertex_stride, vertex_data);

	// Deallocate vertex data
	delete[] face_normals;
	delete[] vertex_data;

	// Update model groups
	subterrain_inside_group->set_index_count(subterrain_mesh->get_faces().size() * 3);
	subterrain_outside_group->set_index_count(subterrain_mesh->get_faces().size() * 3);
}

void subterrain::dig(const float3& position, float radius)
{
	// Construct region containing the cavity sphere
	geom::aabb<float> region = {position, position};
	for (int i = 0; i < 3; ++i)
	{
		region.min_point[i] -= radius + isosurface_resolution;
		region.max_point[i] += radius + isosurface_resolution;
	}

	// Subdivide the octree to the maximum depth within the region
	cube_tree->subdivide_max(region);

	// Query all octree leaf nodes within the region
	std::list<game::system::cube_tree*> nodes;
	cube_tree->visit_leaves(region,
		[&position, radius](game::system::cube_tree& node)
		{
			for (int i = 0; i < 8; ++i)
			{
				// For outside normals (also set node initial distance to +infinity)
				//float distance = math::length(node->corners[i] - position) - radius;
				// if (distance < node->distances[i])

				float distance = radius - math::length(node.corners[i] - position);
				if (distance > node.distances[i])
					node.distances[i] = distance;
			}
		});
}

} // namespace system
} // namespace game

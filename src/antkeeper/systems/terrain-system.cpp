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

#include "terrain-system.hpp"
#include "entity/components/model-component.hpp"
#include "entity/components/collision-component.hpp"
#include "entity/components/transform-component.hpp"
#include "renderer/model.hpp"
#include "geometry/mesh.hpp"
#include "geometry/mesh-functions.hpp"
#include "renderer/vertex-attributes.hpp"
#include "rasterizer/vertex-attribute-type.hpp"
#include "rasterizer/drawing-mode.hpp"
#include "rasterizer/vertex-buffer.hpp"
#include "resources/resource-manager.hpp"
#include "resources/image.hpp"
#include <limits>

using namespace vmq::operators;
using namespace ecs;

terrain_system::terrain_system(entt::registry& registry, ::resource_manager* resource_manager):
	entity_system(registry),
	resource_manager(resource_manager)
{
	registry.on_construct<terrain_component>().connect<&terrain_system::on_terrain_construct>(this);
	registry.on_destroy<terrain_component>().connect<&terrain_system::on_terrain_destroy>(this);

	heightmap = resource_manager->load<image>("grassland-heightmap.png");
	heightmap_size = 2000.0f;
	heightmap_scale = 150.0f;
}

terrain_system::~terrain_system()
{}

void terrain_system::update(double t, double dt)
{
	registry.view<terrain_component, transform_component>().each(
		[this](auto entity, auto& terrain, auto& transform)
		{
			transform.transform.translation = float3{(float)terrain.x * patch_size, 0.0f, (float)terrain.z * patch_size};
			transform.warp = true;
		});
}

void terrain_system::set_patch_size(float size)
{
	patch_size = size;
}

mesh* terrain_system::generate_terrain_mesh(float size, int subdivisions)
{
	// Allocate terrain mesh
	mesh* terrain_mesh = new mesh();

	// Determine vertex count and placement
	int columns = static_cast<int>(std::pow(2, subdivisions));
	int rows = columns;
	int vertex_count = (columns + 1) * (rows + 1);
	float vertex_increment = size / static_cast<float>(columns);
	float radius = size * 0.5f;

	// Generate mesh vertices
	float3 position = {0.0f, 0.0f, -radius};
	for (int i = 0; i <= rows; ++i)
	{
		position[0] = -radius;

		for (int j = 0; j <= columns; ++j)
		{
			terrain_mesh->add_vertex(position);
			position[0] += vertex_increment;

		}

		position[2] += vertex_increment;
	}


	// Function to eliminate duplicate edges
	std::map<std::array<std::size_t, 2>, mesh::edge*> edge_map;
	auto add_or_find_edge = [&](mesh::vertex* start, mesh::vertex* end) -> mesh::edge*
	{
		mesh::edge* edge;
		if (auto it = edge_map.find({start->index, end->index}); it != edge_map.end())
		{
			edge = it->second;
		}
		else
		{
			edge = terrain_mesh->add_edge(start, end);
			edge_map[{start->index, end->index}] = edge;
			edge_map[{end->index, start->index}] = edge->symmetric;
		}

		return edge;
	};

	const std::vector<mesh::vertex*>& vertices = terrain_mesh->get_vertices();
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			mesh::vertex* a = vertices[i * (columns + 1) + j];
			mesh::vertex* b = vertices[(i + 1) * (columns + 1) + j];
			mesh::vertex* c = vertices[i * (columns + 1) + j + 1];
			mesh::vertex* d = vertices[(i + 1) * (columns + 1) + j + 1];

			// +---+---+
			// | \ | / |
			// |---+---|
			// | / | \ |
			// +---+---+
			if ((j % 2) == (i % 2))
			{
				mesh::edge* ab = add_or_find_edge(a, b);
				mesh::edge* bd = add_or_find_edge(b, d);
				mesh::edge* da = add_or_find_edge(d, a);

				mesh::edge* ca = add_or_find_edge(c, a);
				mesh::edge* ad = da->symmetric;
				mesh::edge* dc = add_or_find_edge(d, c);

				// a---c
				// | \ |
				// b---d
				terrain_mesh->add_face({ab, bd, da});
				terrain_mesh->add_face({ca, ad, dc});
			}
			else
			{
				mesh::edge* ab = add_or_find_edge(a, b);
				mesh::edge* bc = add_or_find_edge(b, c);
				mesh::edge* ca = add_or_find_edge(c, a);
				mesh::edge* cb = bc->symmetric;
				mesh::edge* bd = add_or_find_edge(b, d);
				mesh::edge* dc = add_or_find_edge(d, c);

				// a---c
				// | / |
				// b---d
				terrain_mesh->add_face({ab, bc, ca});
				terrain_mesh->add_face({cb, bd, dc});
			}
		}
	}

	return terrain_mesh;
}

model* terrain_system::generate_terrain_model(mesh* terrain_mesh)
{
	// Allocate model
	model* terrain_model = new model();

	// Get model's VAO and VBO
	vertex_buffer* vbo = terrain_model->get_vertex_buffer();
	vertex_array* vao = terrain_model->get_vertex_array();

	// Resize VBO
	int vertex_size = 3 + 3 + 3;
	int vertex_stride = vertex_size * sizeof(float);
	vbo->resize(terrain_mesh->get_faces().size() * 3 * vertex_stride, nullptr);

	// Bind vertex attributes
	std::size_t offset = 0;
	vao->bind_attribute(VERTEX_POSITION_LOCATION, *vbo, 3, vertex_attribute_type::float_32, vertex_stride, 0);
	offset += 3;
	vao->bind_attribute(VERTEX_NORMAL_LOCATION, *vbo, 3, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * offset);
	offset += 3;
	vao->bind_attribute(VERTEX_BARYCENTRIC_LOCATION, *vbo, 3, vertex_attribute_type::float_32, vertex_stride, sizeof(float) * offset);
	offset += 3;
	
	// Create model group
	model_group* model_group = terrain_model->add_group("terrain");
	model_group->set_material(resource_manager->load<material>("grassland-terrain.mtl"));
	model_group->set_drawing_mode(drawing_mode::triangles);
	model_group->set_start_index(0);
	model_group->set_index_count(terrain_mesh->get_faces().size() * 3);

	return terrain_model;
}

void terrain_system::project_terrain_mesh(mesh* terrain_mesh, const terrain_component& component)
{
	
	float offset_x = (float)component.x * patch_size;
	float offset_z = (float)component.z * patch_size;

	for (mesh::vertex* vertex: terrain_mesh->get_vertices())
	{
		int pixel_x = (vertex->position[0] + offset_x + heightmap_size * 0.5f) / heightmap_size * (float)(heightmap->get_width() - 1);
		int pixel_y = (vertex->position[2] + offset_z + heightmap_size * 0.5f) / heightmap_size * (float)(heightmap->get_height() - 1);

		pixel_x = std::max<int>(0, std::min<int>(heightmap->get_width() - 1, pixel_x));
		pixel_y = std::max<int>(0, std::min<int>(heightmap->get_height() - 1, pixel_y));

		int pixel_index = (pixel_y * heightmap->get_width() + pixel_x) * heightmap->get_channels();
		const unsigned char* pixel = static_cast<const unsigned char*>(heightmap->get_pixels()) + pixel_index;

		float elevation = (static_cast<float>(*pixel) / 255.0f - 0.5) * heightmap_scale;
		vertex->position[1] = elevation;
	}
	
}

void terrain_system::update_terrain_model(model* terrain_model, mesh* terrain_mesh)
{
	aabb<float> bounds =
	{
		{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()},
		{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()}
	};
	
	static const float3 barycentric_coords[3] =
	{
		float3{1, 0, 0},
		float3{0, 1, 0},
		float3{0, 0, 1}
	};

	int triangle_count = terrain_mesh->get_faces().size();
	int vertex_count = triangle_count * 3;
	int vertex_size = 3 + 3 + 3;

	// Allocate vertex data
	float* vertex_data = new float[vertex_size * vertex_count];

	// Allocate face normals
	float* face_normals = new float[terrain_mesh->get_faces().size() * 3];
	calculate_face_normals(face_normals, *terrain_mesh);

	// Generate vertex data
	float* v = vertex_data;
	const std::vector<mesh::face*>& faces = terrain_mesh->get_faces();
	for (int i = 0; i < triangle_count; ++i)
	{
		const mesh::face* triangle = faces[i];
		const mesh::vertex* a = triangle->edge->vertex;
		const mesh::vertex* b = triangle->edge->next->vertex;
		const mesh::vertex* c = triangle->edge->previous->vertex;
		const mesh::vertex* abc[] = {a, b, c};

		for (int j = 0; j < 3; ++j)
		{
			const mesh::vertex* vertex = abc[j];

			float3 n = {0, 0, 0};
			mesh::edge* start = vertex->edge;
			mesh::edge* edge = start;
			do
			{
				if (edge->face)
				{
					n += reinterpret_cast<const float3&>(face_normals[edge->face->index * 3]);
				}

				edge = edge->previous->symmetric;
			}
			while (edge != start);
			n = vmq::normalize(n);

			*(v++) = vertex->position[0];
			*(v++) = vertex->position[1];
			*(v++) = vertex->position[2];
			*(v++) = n[0];
			*(v++) = n[1];
			*(v++) = n[2];
			*(v++) = barycentric_coords[j][0];
			*(v++) = barycentric_coords[j][1];
			*(v++) = barycentric_coords[j][2];
			
			// Add position to bounds
			for (int i = 0; i < 3; ++i)
			{
				bounds.min_point[i] = std::min<float>(bounds.min_point[i], vertex->position[i]);
				bounds.max_point[i] = std::max<float>(bounds.max_point[i], vertex->position[i]);
			}
		}
	}
	
	// Update bounds
	terrain_model->set_bounds(bounds);

	// Update VBO
	terrain_model->get_vertex_buffer()->update(0, vertex_count * vertex_size * sizeof(float), vertex_data);

	// Free vertex data
	delete[] face_normals;
	delete[] vertex_data;
}

void terrain_system::on_terrain_construct(entt::registry& registry, entt::entity entity, terrain_component& component)
{
	mesh* terrain_mesh = generate_terrain_mesh(patch_size, component.subdivisions);
	model* terrain_model = generate_terrain_model(terrain_mesh);
	project_terrain_mesh(terrain_mesh, component);
	update_terrain_model(terrain_model, terrain_mesh);

	// Assign the entity a collision component with the terrain mesh
	collision_component collision;
	collision.mesh = terrain_mesh;
	collision.bounds = calculate_bounds(*terrain_mesh);
	collision.mesh_accelerator.build(*collision.mesh);
	registry.assign_or_replace<collision_component>(entity, collision);

	// Assign the entity a model component with the terrain model
	model_component model;
	model.model = terrain_model;
	model.instance_count = 0;
	registry.assign_or_replace<model_component>(entity, model);

	// Assign the entity a transform component
	transform_component transform;
	transform.transform = vmq::identity_transform<float>;
	transform.transform.translation = float3{(float)component.x * patch_size, 0.0f, (float)component.z * patch_size};
	transform.warp = true;
	registry.assign_or_replace<transform_component>(entity, transform);
}

void terrain_system::on_terrain_destroy(entt::registry& registry, entt::entity entity)
{
	/*
	if (auto it = terrain_map.find(entity); it != terrain_map.end())
	{
		delete std::get<0>(it->second);
		delete std::get<1>(it->second);
		terrain_map.erase(it);
	}
	*/
}


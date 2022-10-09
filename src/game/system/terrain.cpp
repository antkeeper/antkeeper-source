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

#include "game/system/terrain.hpp"
#include "game/component/celestial-body.hpp"
#include "game/component/observer.hpp"
#include "game/component/terrain.hpp"
#include "geom/meshes/grid.hpp"
#include "geom/mesh-functions.hpp"
#include "geom/morton.hpp"
#include "geom/quadtree.hpp"
#include "geom/spherical.hpp"
#include "gl/vertex-attribute.hpp"
#include "math/constants.hpp"
#include "math/quaternion-operators.hpp"
#include "render/vertex-attribute.hpp"
#include "utility/fundamental-types.hpp"
#include <functional>
#include <iostream>

namespace game {
namespace system {

terrain::terrain(entity::registry& registry):
	updatable(registry),
	patch_subdivisions(0),
	patch_base_mesh(nullptr),
	patch_vertex_size(0),
	patch_vertex_count(0),
	patch_vertex_data(nullptr),
	patch_scene_collection(nullptr),
	max_error(0.0)
{
	// Build set of quaternions to rotate quadtree cube coordinates into BCBF space according to face index
	face_rotations[0] = math::quaternion<double>::identity;                       // +x
	face_rotations[1] = math::quaternion<double>::rotate_z(math::pi<double>);       // -x
	face_rotations[2] = math::quaternion<double>::rotate_z( math::half_pi<double>); // +y
	face_rotations[3] = math::quaternion<double>::rotate_z(-math::half_pi<double>); // -y
	face_rotations[4] = math::quaternion<double>::rotate_y(-math::half_pi<double>); // +z
	face_rotations[5] = math::quaternion<double>::rotate_y( math::half_pi<double>); // -z
	
	// Specify vertex size and stride
	// (position + uv + normal + tangent + barycentric + target)
	patch_vertex_size = 3 + 2 + 3 + 4 + 3 + 3;
	patch_vertex_stride = patch_vertex_size * sizeof(float);
	
	// Init patch subdivisions to zero
	set_patch_subdivisions(0);
	
	registry.on_construct<component::terrain>().connect<&terrain::on_terrain_construct>(this);
	registry.on_destroy<component::terrain>().connect<&terrain::on_terrain_destroy>(this);
}

terrain::~terrain()
{
	registry.on_construct<component::terrain>().disconnect<&terrain::on_terrain_construct>(this);
	registry.on_destroy<component::terrain>().disconnect<&terrain::on_terrain_destroy>(this);
}

void terrain::update(double t, double dt)
{
	/*
	// Refine the level of detail of each terrain quadsphere
	registry.view<component::terrain, component::celestial_body>().each(
	[&](entity::id terrain_eid, const auto& terrain_component, const auto& terrain_body)
	{
		// Retrieve terrain quadsphere
		terrain_quadsphere* quadsphere = terrain_quadspheres[terrain_eid];
		
		// For each observer
		this->registry.view<component::observer>().each(
		[&](entity::id observer_eid, const auto& observer)
		{
			// Skip observers with invalid reference body
			if (!this->registry.has<component::celestial_body>(observer.reference_body_eid) ||
				!this->registry.has<component::terrain>(observer.reference_body_eid))
				return;
			
			// Get celestial body and terrain component of observer reference body
			const auto& reference_celestial_body = this->registry.get<component::celestial_body>(observer.reference_body_eid);
			const auto& reference_terrain = this->registry.get<component::terrain>(observer.reference_body_eid);
			
			// Calculate reference BCBF-space position of observer.
			//double3 observer_spherical = {reference_celestial_body.radius + observer.elevation, observer.latitude, observer.longitude};
			double3 observer_spherical = {observer.elevation, observer.latitude, observer.longitude};
			double3 observer_cartesian = geom::spherical::to_cartesian(observer_spherical);
			
			observer_cartesian = math::type_cast<double>(observer.camera->get_translation());
			
			/// @TODO Transform observer position into BCBF space of terrain body (use orbit component?)
			
			// For each terrain quadsphere face
			for (int i = 0; i < 6; ++i)
			{
				terrain_quadsphere_face& quadsphere_face = quadsphere->faces[i];

				// Get the quadsphere faces's quadtree
				quadtree_type& quadtree = quadsphere_face.quadtree;
				
				// Clear quadsphere face quadtree
				quadtree.clear();
				
				// For each node in the face quadtree
				for (auto node_it = quadtree.begin(); node_it != quadtree.end(); ++node_it)
				{
					quadtree_node_type node = *node_it;
					
					// Skip non leaf nodes
					if (!quadtree.is_leaf(node))
						continue;
					
					// Extract node depth
					quadtree_type::node_type node_depth = quadtree_type::depth(node);
					
					// Skip nodes at max depth level
					if (node_depth >= terrain_component.max_lod)
						continue;
					
					// Extract node location from Morton location code
					quadtree_type::node_type node_location = quadtree_type::location(node);
					quadtree_type::node_type node_location_x;
					quadtree_type::node_type node_location_y;
					geom::morton::decode(node_location, node_location_x, node_location_y);
					

					
					const double nodes_per_axis = std::exp2(node_depth);
					const double node_width = 2.0 / nodes_per_axis;
					
					// Determine node center on front face of unit BCBF cube.
					double3 center;
					center.y = -(nodes_per_axis * 0.5 * node_width) + node_width * 0.5;
					center.z = center.y;
					center.y += static_cast<double>(node_location_x) * node_width;
					center.z += static_cast<double>(node_location_y) * node_width;
					center.x = 1.0;
					
					// Rotate node center according to cube face
					/// @TODO Rather than rotating every center, "unrotate" observer position 6 times
					center = face_rotations[i] * center;
					
					// Project node center onto unit sphere
					double xx = center.x * center.x;
					double yy = center.y * center.y;
					double zz = center.z * center.z;
					center.x *= std::sqrt(std::max(0.0, 1.0 - yy * 0.5 - zz * 0.5 + yy * zz / 3.0));
					center.y *= std::sqrt(std::max(0.0, 1.0 - xx * 0.5 - zz * 0.5 + xx * zz / 3.0));
					center.z *= std::sqrt(std::max(0.0, 1.0 - xx * 0.5 - yy * 0.5 + xx * yy / 3.0));
					
					// Scale node center by body radius
					center *= terrain_body.radius;
					center.y -= terrain_body.radius;
					//center *= 50.0;
					
					const double horizontal_fov = observer.camera->get_fov();
					const double horizontal_resolution = 1920.0;
					const double distance = math::length(center - observer_cartesian);
					const double geometric_error = static_cast<double>(524288.0 / std::exp2(node_depth));
					const double screen_error = screen_space_error(horizontal_fov, horizontal_resolution, distance, geometric_error);
					
					if (screen_error > max_error)
					{
						//std::cout << screen_error << std::endl;
						quadtree.insert(quadtree_type::child(node, 0));
					}
				}
			}
		});
	});
	
	// Handle meshes and models for each terrain patch
	registry.view<component::terrain, component::celestial_body>().each(
	[&](entity::id terrain_eid, const auto& terrain_component, const auto& terrain_body)
	{
		// Retrieve terrain quadsphere
		terrain_quadsphere* quadsphere = terrain_quadspheres[terrain_eid];
		
		// For each terrain quadsphere face
		for (int i = 0; i < 6; ++i)
		{
			terrain_quadsphere_face& quadsphere_face = quadsphere->faces[i];
			const quadtree_type& quadtree = quadsphere_face.quadtree;

			// For each quadtree node
			for (auto node_it = quadtree.unordered_begin(); node_it != quadtree.unordered_end(); ++node_it)
			{
				quadtree_node_type node = *node_it;
				
				// Look up cached patch for this node
				auto patch_it = quadsphere_face.patches.find(node);
				
				// If there is no cached patch instance for this node
				if (patch_it == quadsphere_face.patches.end())
				{
					// Construct a terrain patch
					terrain_patch* patch = new terrain_patch();
					
					// Generate a patch mesh
					patch->mesh = generate_patch_mesh(i, *node_it, terrain_body.radius, terrain_component.elevation);
					//patch->mesh = generate_patch_mesh(i, *node_it, 50.0, terrain_component.elevation);
					
					// Generate a patch model
					patch->model = generate_patch_model(*patch->mesh, terrain_component.patch_material);
					
					// Construct patch model instance
					patch->model_instance = new scene::model_instance(patch->model);

					
					// Cache the terrain patch
					quadsphere_face.patches[node] = patch;
					
					// Add patch model instance to the patch scene collection
					if (patch_scene_collection)
						patch_scene_collection->add_object(patch->model_instance);
				}
			}
			
			// For each terrain pach
			for (auto patch_it = quadsphere_face.patches.begin(); patch_it != quadsphere_face.patches.end(); ++patch_it)
			{
				quadtree_node_type node = patch_it->first;
				
				// Set patch model instance active if its node is a leaf node, otherwise deactivate it
				bool active = (quadtree.contains(node) && quadtree.is_leaf(node));
				patch_it->second->model_instance->set_active(active);
			}
		}
	});
	*/
}

void terrain::set_patch_subdivisions(std::uint8_t n)
{
	patch_subdivisions = n;
	
	// Rebuid patch base mesh
	{
		delete patch_base_mesh;
		patch_base_mesh = geom::meshes::grid_xy(2.0f, patch_subdivisions, patch_subdivisions);
		
		// Convert quads to triangle fans
		for (std::size_t i = 0; i < patch_base_mesh->get_faces().size(); ++i)
		{
			geom::mesh::face* face = patch_base_mesh->get_faces()[i];
			
			std::size_t edge_count = 1;
			for (geom::mesh::edge* edge = face->edge->next; edge != face->edge; edge = edge->next)
				++edge_count;
			
			if (edge_count > 3)
			{
				geom::poke_face(*patch_base_mesh, face->index);
				--i;
			}
		}
	}
	
	// Transform patch base mesh coordinates to match the front face of a BCBF cube
	const math::quaternion<float> xy_to_zy = math::quaternion<float>::rotate_y(-math::half_pi<float>);
	for (geom::mesh::vertex* vertex: patch_base_mesh->get_vertices())
	{
		vertex->position = xy_to_zy * vertex->position;
		vertex->position.x = 1.0f;
	}
	
	// Recalculate number of vertices per patch
	patch_vertex_count = patch_base_mesh->get_faces().size() * 3;
	
	// Resize patch vertex data buffer
	delete[] patch_vertex_data;
	patch_vertex_data = new float[patch_vertex_count * patch_vertex_size];
}

void terrain::set_patch_scene_collection(scene::collection* collection)
{
	patch_scene_collection = collection;
}

void terrain::set_max_error(double error)
{
	max_error = error;
}

void terrain::on_terrain_construct(entity::registry& registry, entity::id entity_id)
{
	terrain_quadsphere* quadsphere = new terrain_quadsphere();
	terrain_quadspheres[entity_id] = quadsphere;
}

void terrain::on_terrain_destroy(entity::registry& registry, entity::id entity_id)
{
	// Find terrain quadsphere for the given entity ID
	auto quadsphere_it = terrain_quadspheres.find(entity_id);
	
	if (quadsphere_it != terrain_quadspheres.end())
	{
		terrain_quadsphere* quadsphere = quadsphere_it->second;
		
		// For each terrain quadsphere face
		for (int i = 0; i < 6; ++i)
		{
			terrain_quadsphere_face& quadsphere_face = quadsphere->faces[i];
			
			for (auto patch_it = quadsphere_face.patches.begin(); patch_it != quadsphere_face.patches.end(); ++patch_it)
			{
				terrain_patch* patch = patch_it->second;
				
				if (patch_scene_collection)
					patch_scene_collection->remove_object(patch->model_instance);
				
				delete patch->model_instance;
				delete patch->model;
				delete patch->mesh;
				
				delete patch;
			}
		}
		
		// Free terrain quadsphere
		delete quadsphere;
	}
	
	// Remove terrain quadsphere from the map
	terrain_quadspheres.erase(quadsphere_it);
}

geom::mesh* terrain::generate_patch_mesh(std::uint8_t face_index, quadtree_node_type node, double body_radius, const std::function<double(double, double)>& elevation) const
{
	// Extract node depth
	const quadtree_type::node_type depth = quadtree_type::depth(node);
	
	// Extract node Morton location code and decode location
	const quadtree_type::node_type location = quadtree_type::location(node);
	quadtree_type::node_type location_x;
	quadtree_type::node_type location_y;
	geom::morton::decode(location, location_x, location_y);
	
	const double nodes_per_axis = std::exp2(depth);
	
	const double scale_yz = 1.0 / nodes_per_axis;
	
	const double node_width = 2.0 / nodes_per_axis;
	
	// Determine vertex offset according to node location
	double offset_y = -(nodes_per_axis * 0.5 * node_width) + node_width * 0.5;
	double offset_z = offset_y;
	offset_y += static_cast<double>(location_x) * node_width;
	offset_z += static_cast<double>(location_y) * node_width;
	
	// Copy base mesh
	geom::mesh* patch_mesh = new geom::mesh(*patch_base_mesh);
	
	// Modify patch mesh vertex positions
	for (geom::mesh::vertex* v: patch_mesh->get_vertices())
	{
		double3 position = math::type_cast<double>(v->position);
		
		// Offset and scale vertex position
		position.y *= scale_yz;
		position.z *= scale_yz;
		position.y += offset_y;
		position.z += offset_z;

		// Rotate according to cube face
		position = face_rotations[face_index] * position;
		
		// Project onto unit sphere
		//position = math::normalize(position);
		
		// Cartesian Spherical Cube projection (KSC)
		/// @see https://catlikecoding.com/unity/tutorials/cube-sphere/
		/// @see https://core.ac.uk/download/pdf/228552506.pdf
		double xx = position.x * position.x;
		double yy = position.y * position.y;
		double zz = position.z * position.z;
		position.x *= std::sqrt(std::max(0.0, 1.0 - yy * 0.5 - zz * 0.5 + yy * zz / 3.0));
		position.y *= std::sqrt(std::max(0.0, 1.0 - xx * 0.5 - zz * 0.5 + xx * zz / 3.0));
		position.z *= std::sqrt(std::max(0.0, 1.0 - xx * 0.5 - yy * 0.5 + xx * yy / 3.0));
		
		// Calculate latitude and longitude of vertex position
		const double latitude = std::atan2(position.z, std::sqrt(position.x * position.x + position.y * position.y));
		const double longitude = std::atan2(position.y, position.x);
		
		// Look up elevation at latitude and longitude and use to calculate radial distance
		const double radial_distance = body_radius + elevation(latitude, longitude);
		
		// Scale vertex position by radial distance
		position *= radial_distance;
		position.y -= body_radius;
		
		v->position = math::type_cast<float>(position);
	}
	
	return patch_mesh;
}

::render::model* terrain::generate_patch_model(const geom::mesh& patch_mesh, ::render::material* patch_material) const
{
	// Barycentric coordinates
	static const float3 barycentric[3] =
	{
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};
	
	// Fill vertex data buffer
	float* v = patch_vertex_data;
	for (const geom::mesh::face* face: patch_mesh.get_faces())
	{
		const geom::mesh::vertex* a = face->edge->vertex;
		const geom::mesh::vertex* b = face->edge->next->vertex;
		const geom::mesh::vertex* c = face->edge->previous->vertex;
		const geom::mesh::vertex* face_vertices[] = {a, b, c};
		
		// Calculate facted normal
		float3 normal = math::normalize(math::cross(b->position - a->position, c->position - a->position));
		
		for (std::size_t i = 0; i < 3; ++i)
		{
			const geom::mesh::vertex* vertex = face_vertices[i];
			
			// Vertex position
			const float3& position = vertex->position;
			*(v++) = position.x;
			*(v++) = position.y;
			*(v++) = position.z;
			
			// Vertex UV coordinates (latitude, longitude)
			const float latitude = std::atan2(position.z, std::sqrt(position.x * position.x + position.y * position.y));
			const float longitude = std::atan2(position.y, position.x);
			*(v++) = latitude;
			*(v++) = longitude;
			
			// Vertex normal
			*(v++) = normal.x;
			*(v++) = normal.y;
			*(v++) = normal.z;
			
			/// @TODO Vertex tangent
			*(v++) = 0.0f;
			*(v++) = 0.0f;
			*(v++) = 0.0f;
			*(v++) = 0.0f;
			
			// Vertex barycentric coordinates
			*(v++) = barycentric[i].x;
			*(v++) = barycentric[i].y;
			*(v++) = barycentric[i].z;
			
			// Vertex morph target (LOD transition)
			*(v++) = 0.0f;
			*(v++) = 0.0f;
			*(v++) = 0.0f;
		}
	}
	
	// Get triangle count of patch mesh
	std::size_t patch_triangle_count = patch_mesh.get_faces().size();
	
	// Allocate patch model
	::render::model* patch_model = new ::render::model();
	
	// Get model VBO and VAO
	gl::vertex_buffer* vbo = patch_model->get_vertex_buffer();
	gl::vertex_array* vao = patch_model->get_vertex_array();

	// Resize model VBO and upload vertex data
	vbo->resize(patch_triangle_count * 3 * patch_vertex_stride, patch_vertex_data);
	
	std::size_t attribute_offset = 0;
	
	// Define position vertex attribute
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo;
	position_attribute.offset = attribute_offset;
	position_attribute.stride = patch_vertex_stride;
	position_attribute.type = gl::vertex_attribute_type::float_32;
	position_attribute.components = 3;
	attribute_offset += position_attribute.components * sizeof(float);
	
	// Define UV vertex attribute
	gl::vertex_attribute uv_attribute;
	uv_attribute.buffer = vbo;
	uv_attribute.offset = attribute_offset;
	uv_attribute.stride = patch_vertex_stride;
	uv_attribute.type = gl::vertex_attribute_type::float_32;
	uv_attribute.components = 2;
	attribute_offset += uv_attribute.components * sizeof(float);
	
	// Define normal vertex attribute
	gl::vertex_attribute normal_attribute;
	normal_attribute.buffer = vbo;
	normal_attribute.offset = attribute_offset;
	normal_attribute.stride = patch_vertex_stride;
	normal_attribute.type = gl::vertex_attribute_type::float_32;
	normal_attribute.components = 3;
	attribute_offset += normal_attribute.components * sizeof(float);
	
	// Define tangent vertex attribute
	gl::vertex_attribute tangent_attribute;
	tangent_attribute.buffer = vbo;
	tangent_attribute.offset = attribute_offset;
	tangent_attribute.stride = patch_vertex_stride;
	tangent_attribute.type = gl::vertex_attribute_type::float_32;
	tangent_attribute.components = 4;
	attribute_offset += tangent_attribute.components * sizeof(float);
	
	// Define barycentric vertex attribute
	gl::vertex_attribute barycentric_attribute;
	barycentric_attribute.buffer = vbo;
	barycentric_attribute.offset = attribute_offset;
	barycentric_attribute.stride = patch_vertex_stride;
	barycentric_attribute.type = gl::vertex_attribute_type::float_32;
	barycentric_attribute.components = 3;
	attribute_offset += barycentric_attribute.components * sizeof(float);
	
	// Define target vertex attribute
	gl::vertex_attribute target_attribute;
	target_attribute.buffer = vbo;
	target_attribute.offset = attribute_offset;
	target_attribute.stride = patch_vertex_stride;
	target_attribute.type = gl::vertex_attribute_type::float_32;
	target_attribute.components = 3;
	attribute_offset += target_attribute.components * sizeof(float);
	
	// Bind vertex attributes to VAO
	vao->bind(::render::vertex_attribute::position, position_attribute);
	vao->bind(::render::vertex_attribute::uv, uv_attribute);
	vao->bind(::render::vertex_attribute::normal, normal_attribute);
	vao->bind(::render::vertex_attribute::tangent, tangent_attribute);
	vao->bind(::render::vertex_attribute::barycentric, barycentric_attribute);
	vao->bind(::render::vertex_attribute::target, target_attribute);
	
	// Create model group
	::render::model_group* patch_model_group = patch_model->add_group("terrain");
	patch_model_group->set_material(patch_material);
	patch_model_group->set_drawing_mode(gl::drawing_mode::triangles);
	patch_model_group->set_start_index(0);
	patch_model_group->set_index_count(patch_triangle_count * 3);
	
	// Calculate model bounds
	geom::aabb<float> bounds = geom::calculate_bounds(patch_mesh);
	patch_model->set_bounds(bounds);
	
	return patch_model;
}

double terrain::screen_space_error(double horizontal_fov, double horizontal_resolution, double distance, double geometric_error)
{
	// Calculate view frustum width at given distance
	const double frustum_width = 2.0 * distance * std::tan(horizontal_fov * 0.5);
	
	return (geometric_error * horizontal_resolution) / frustum_width;
}

} // namespace system
} // namespace game

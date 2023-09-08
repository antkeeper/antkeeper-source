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

#include "game/systems/terrain-system.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-component.hpp"
#include <engine/debug/log.hpp>
#include <engine/geom/primitives/box.hpp>
#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/geom/brep/brep-operations.hpp>
#include <engine/physics/kinematics/rigid-body.hpp>
#include <engine/physics/kinematics/collider.hpp>
#include <engine/physics/kinematics/colliders/mesh-collider.hpp>
#include <engine/scene/static-mesh.hpp>
#include <engine/render/vertex-attribute.hpp>
#include <algorithm>
#include <execution>
#include <stdexcept>

terrain_system::terrain_system(entity::registry& registry):
	updatable_system(registry)
{}

terrain_system::~terrain_system()
{}

void terrain_system::update(float t, float dt)
{
}

entity::id terrain_system::generate(std::shared_ptr<image> heightmap, const math::uvec2& subdivisions, const math::transform<float>& transform, std::shared_ptr<render::material> material)
{
	if (!heightmap)
	{
		debug::log::error("Failed to generate terrain from null heightmap");
		throw std::invalid_argument("Failed to generate terrain from null heightmap");
	}
	
	if (heightmap->size().x() < 2 || heightmap->size().y() < 2)
	{
		debug::log::error("Heightmap size less than 2x2");
		throw std::runtime_error("Heightmap size less than 2x2");
	}
	
	if (((heightmap->size().x() - 1) % (subdivisions.x() + 1)) != 0 || 
		((heightmap->size().y() - 1) % (subdivisions.y() + 1)) != 0)
	{
		debug::log::error("{}x{} heightmap cannot be subdivided {}x{} times",
			heightmap->size().x(),
			heightmap->size().y(),
			subdivisions.x(),
			subdivisions.y());
		throw std::runtime_error("Heightmap subdivision failed");
	}
	
	// Generate terrain grid
	terrain_grid_component grid;
	grid.dimensions = subdivisions + 1u;
	grid.cells.resize(grid.dimensions.x() * grid.dimensions.y());
	auto grid_eid = registry.create();
	for (auto y = 0u; y < grid.dimensions.y(); ++y)
	{
		for (auto x = 0u; x < grid.dimensions.x(); ++x)
		{
			auto cell_eid = registry.create();
			registry.emplace<terrain_cell_component>(cell_eid, grid_eid, math::uvec2{x, y});
			grid.cells[y * grid.dimensions.x() + x] = cell_eid;
		}
	}
	
	// Calculate cell dimensions
	const auto cell_quad_dimensions = math::uvec2{static_cast<unsigned int>(heightmap->size().x() - 1) / grid.dimensions.x(), static_cast<unsigned int>(heightmap->size().y() - 1) / grid.dimensions.y()};
	const auto cell_vert_dimensions = cell_quad_dimensions + 1u;
	
	const auto max_scale = math::max(transform.scale);
	const auto scale_ratio = transform.scale / max_scale;
	const auto vertex_scale = scale_ratio * math::fvec3{2.0f / static_cast<float>(cell_quad_dimensions.x()), 2.0f, 2.0f / static_cast<float>(cell_quad_dimensions.y())};
	const auto vertex_translation = -scale_ratio;
	
	// Generate terrain cell meshes
	std::for_each
	(
		std::execution::seq,
		std::begin(grid.cells),
		std::end(grid.cells),
		[&](auto cell_eid)
		{
			const auto& cell = registry.get<terrain_cell_component>(cell_eid);
			
			// Allocate cell mesh and attributes
			auto mesh = std::make_shared<geom::brep_mesh>();
			auto& vertex_positions = static_cast<geom::brep_attribute<math::fvec3>&>(*mesh->vertices().attributes().emplace<math::fvec3>("position"));
			
			auto cell_pixel_bounds_min = cell.coordinates * cell_quad_dimensions;
			auto cell_pixel_bounds_max = cell_pixel_bounds_min + cell_quad_dimensions;
			
			// Build cell vertices
			math::uvec2 pixel_position;
			for (pixel_position.y() = cell_pixel_bounds_min.y(); pixel_position.y() <= cell_pixel_bounds_max.y(); ++pixel_position.y())
			{
				for (pixel_position.x() = cell_pixel_bounds_min.x(); pixel_position.x() <= cell_pixel_bounds_max.x(); ++pixel_position.x())
				{
					// Allocate vertex
					auto vertex = mesh->vertices().emplace_back();
					
					// Get vertex height from heightmap
					float height = heightmap->sample(pixel_position).x();
					
					// Set vertex position
					auto& position = vertex_positions[vertex->index()];
					position.x() = static_cast<float>(pixel_position.x()) * vertex_scale.x() + vertex_translation.x();
					position.y() = height * vertex_scale.y() + vertex_translation.y();
					position.z() = static_cast<float>(pixel_position.y()) * vertex_scale.z() + vertex_translation.z();
				}
			}
			
			// Build cell faces
			for (auto y = 0u; y < cell_quad_dimensions.y(); ++y)
			{
				for (auto x = 0u; x < cell_quad_dimensions.x(); ++x)
				{
					auto a = mesh->vertices()[y * cell_vert_dimensions.x() + x];
					auto b = mesh->vertices()[a->index() + cell_vert_dimensions.x()];
					auto c = mesh->vertices()[a->index() + 1];
					auto d = mesh->vertices()[b->index() + 1];
					
					geom::brep_vertex* abc[3] = {a, b, c};
					geom::brep_vertex* cbd[3] = {c, b, d};
					
					mesh->faces().emplace_back(abc);
					mesh->faces().emplace_back(cbd);
				}
			}
			
			// Generate vertex normals
			auto& vertex_normals = static_cast<geom::brep_attribute<math::fvec3>&>(*mesh->vertices().attributes().try_emplace<math::fvec3>("normal").first);
			for (pixel_position.y() = cell_pixel_bounds_min.y(); pixel_position.y() <= cell_pixel_bounds_max.y(); ++pixel_position.y())
			{
				for (pixel_position.x() = cell_pixel_bounds_min.x(); pixel_position.x() <= cell_pixel_bounds_max.x(); ++pixel_position.x())
				{
					const auto pixel_w = pixel_position.x() >= 1u ? pixel_position - math::uvec2{1, 0} : pixel_position;
					const auto pixel_e = pixel_position.x() < cell_pixel_bounds_max.x() ? pixel_position + math::uvec2{1, 0} : pixel_position;
					const auto pixel_s = pixel_position.y() >= 1u ? pixel_position - math::uvec2{0, 1} : pixel_position;
					const auto pixel_n = pixel_position.y() < cell_pixel_bounds_max.y() ? pixel_position + math::uvec2{0, 1} : pixel_position;
					
					const auto index_c = pixel_position.y() * (cell_pixel_bounds_max.x() + 1) + pixel_position.x();
					const auto index_w = pixel_w.y() * (cell_pixel_bounds_max.x() + 1) + pixel_w.x();
					const auto index_e = pixel_e.y() * (cell_pixel_bounds_max.x() + 1) + pixel_e.x();
					const auto index_s = pixel_s.y() * (cell_pixel_bounds_max.x() + 1) + pixel_s.x();
					const auto index_n = pixel_n.y() * (cell_pixel_bounds_max.x() + 1) + pixel_n.x();
					
					const auto height_w = vertex_positions[index_w].y();
					const auto height_e = vertex_positions[index_e].y();
					const auto height_s = vertex_positions[index_s].y();
					const auto height_n = vertex_positions[index_n].y();
					
					// float height_w = heightmap->sample(pixel_w).x();
					// float height_e = heightmap->sample(pixel_e).x();
					// float height_s = heightmap->sample(pixel_s).x();
					// float height_n = heightmap->sample(pixel_n).x();
					
					auto& normal_c = vertex_normals[index_c];
					normal_c = math::normalize(math::fvec3{(height_w - height_e) / vertex_scale.x(), 2.0f, (height_s - height_n) / vertex_scale.z()});
				}
			}
			
			// Construct terrain cell rigid body
			auto rigid_body = std::make_unique<physics::rigid_body>();
			rigid_body->set_mass(0.0f);
			rigid_body->set_collider(std::make_shared<physics::mesh_collider>(mesh));
			rigid_body->set_transform({transform.translation, transform.rotation, math::fvec3{max_scale, max_scale, max_scale} * 0.5f});
			registry.emplace<rigid_body_component>(cell_eid, std::move(rigid_body));
			
			auto model = generate_terrain_model(*mesh, material, cell_quad_dimensions);
			scene_component scene;
			scene.object = std::make_shared<scene::static_mesh>(std::move(model));
			scene.layer_mask = 1;
			registry.emplace<scene_component>(cell_eid, std::move(scene));
		}
	);
	
	registry.emplace<terrain_grid_component>(grid_eid, std::move(grid));
	return grid_eid;
}

std::unique_ptr<render::model> terrain_system::generate_terrain_model(const geom::brep_mesh& mesh, std::shared_ptr<render::material> material, const math::uvec2& quad_dimensions) const
{
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	const auto& vertex_normals = mesh.vertices().attributes().at<math::fvec3>("normal");
	
	// Allocate model
	auto model = std::make_unique<render::model>();
	
	// Init model bounds
	auto& bounds = model->get_bounds();
	bounds = {math::fvec3::infinity(), -math::fvec3::infinity()};
	
	// Get model VBO and VAO
	auto& vbo = model->get_vertex_buffer();
	auto& vao = model->get_vertex_array();
	
	// Build vertex format
	const std::size_t vertex_size = 3 * sizeof(std::int16_t) + 3 * sizeof(float);
	gl::vertex_attribute position_attribute;
	position_attribute.buffer = vbo.get();
	position_attribute.offset = 0;
	position_attribute.stride = vertex_size;
	position_attribute.type = gl::vertex_attribute_type::int_16;
	position_attribute.components = 3;
	position_attribute.normalized = true;
	gl::vertex_attribute normal_attribute;
	normal_attribute.buffer = vbo.get();
	normal_attribute.offset = 3 * sizeof(std::int16_t);
	normal_attribute.stride = vertex_size;
	normal_attribute.type = gl::vertex_attribute_type::float_32;
	normal_attribute.components = 3;
	
	const auto vert_dimensions = quad_dimensions + 1u;
	
	// Interleave vertex data
	const std::size_t vertex_count = 2 * (vert_dimensions.x() * quad_dimensions.y() + quad_dimensions.y() - 1);
	std::vector<std::byte> vertex_data(vertex_count * vertex_size);
	std::byte* v = vertex_data.data();
	
	auto normalized_int16 = [](const math::fvec3& f) -> math::vec3<std::int16_t>
	{
		math::vec3<std::int16_t> i;
		for (int j = 0; j < 3; ++j)
		{
			i[j] = static_cast<std::int16_t>(f[j] < 0.0f ? f[j] * 32768.0f : f[j] * 32767.0f);
		}
		return i;
	};
	
	for (auto y = 0u; y < quad_dimensions.y(); ++y)
	{
		std::size_t indices[2];
		
		for (auto x = 0u; x < vert_dimensions.x(); ++x)
		{
			indices[0] = y * vert_dimensions.x() + x;
			indices[1] = indices[0] + vert_dimensions.x();
			
			for (auto i: indices)
			{
				auto position = normalized_int16(vertex_positions[i]);
				
				std::memcpy(v, &position[0], sizeof(std::int16_t) * 3);
				v += sizeof(std::int16_t) * 3;
				std::memcpy(v, &vertex_normals[i], sizeof(float) * 3);
				v += sizeof(float) * 3;
				
				// Extend model bounds
				bounds.extend(vertex_positions[i]);
			}
		}
		
		if (y < quad_dimensions.y() - 1)
		{
			// Restart triangle strip on next row using degenerate triangles
			
			
			auto position = normalized_int16(vertex_positions[indices[1]]);
			std::memcpy(v, &position[0], sizeof(std::int16_t) * 3);
			v += sizeof(int16_t) * 3;
			std::memcpy(v, &vertex_normals[indices[1]], sizeof(float) * 3);
			v += sizeof(float) * 3;
			
			indices[0] = (y + 1) * vert_dimensions.x();
			
			position = normalized_int16(vertex_positions[indices[0]]);
			std::memcpy(v, &position[0], sizeof(std::int16_t) * 3);
			v += sizeof(int16_t) * 3;
			std::memcpy(v, &vertex_normals[indices[0]], sizeof(float) * 3);
			v += sizeof(float) * 3;
		}
	}
	
	// Resize model VBO and upload interleaved vertex data
	vbo->resize(vertex_data.size(), vertex_data);
	
	// Free interleaved vertex data
	vertex_data.clear();
	
	// Bind vertex attributes to VAO
	vao->bind(render::vertex_attribute::position, position_attribute);
	vao->bind(render::vertex_attribute::normal, normal_attribute);
	
	// Create material group
	model->get_groups().resize(1);
	render::model_group& model_group = model->get_groups().front();
	
	model_group.id = {};
	model_group.material = material;
	model_group.drawing_mode = gl::drawing_mode::triangle_strip;
	model_group.start_index = 0;
	model_group.index_count = static_cast<std::uint32_t>(vertex_count);
	
	return model;
}

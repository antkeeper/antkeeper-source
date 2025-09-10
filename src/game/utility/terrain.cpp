// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <entt/entt.hpp>
#include "game/utility/terrain.hpp"
#include "game/components/terrain-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/scene-object-component.hpp"
#include <engine/gl/pipeline.hpp>
#include <engine/render/model.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/physics/kinematics/colliders/mesh-collider.hpp>
#include <engine/scene/static-mesh.hpp>
#include <engine/debug/log.hpp>
#include <engine/geom/brep/mesh.hpp>
#include <engine/utility/sized-types.hpp>

using namespace engine;
using namespace engine::geom;

namespace
{
	[[nodiscard]] std::unique_ptr<render::model> generate_terrain_model(const brep::mesh& mesh, std::shared_ptr<render::material> material, const math::uvec2& quad_dimensions)
	{
		const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
		const auto& vertex_normals = mesh.vertices().attributes().at<math::fvec3>("normal");

		// Allocate model
		auto model = std::make_unique<render::model>();

		// Add material to model
		model->materials().emplace_back(std::move(material));

		// Init model bounds
		auto& bounds = model->get_bounds();
		bounds = {math::inf<math::fvec3>, -math::inf<math::fvec3>};

		// Construct VAO
		constexpr gl::vertex_input_attribute vertex_attributes[] =
		{
			{
				render::vertex_attribute_location::position,
				0,
				gl::format::r16g16b16_snorm,
				0
			},
			{
				render::vertex_attribute_location::normal,
				0,
				gl::format::r32g32b32_sfloat,
				3 * sizeof(i16)
			}
		};
		auto& vao = model->get_vertex_array();
		vao = std::make_unique<gl::vertex_array>(vertex_attributes);

		// Interleave vertex data
		const auto vert_dimensions = quad_dimensions + 1u;
		const usize vertex_count = 2 * (vert_dimensions.x() * quad_dimensions.y() + quad_dimensions.y() - 1);
		constexpr usize vertex_stride = 3 * sizeof(i16) + 3 * sizeof(float);
		std::vector<std::byte> vertex_data(vertex_count * vertex_stride);
		std::byte* v = vertex_data.data();

		auto normalized_int16 = [](const math::fvec3& f) -> math::vec3<i16>
			{
				math::vec3<i16> i;
				for (int j = 0; j < 3; ++j)
				{
					i[j] = static_cast<i16>(f[j] < 0.0f ? f[j] * 32768.0f : f[j] * 32767.0f);
				}
				return i;
			};

		for (auto y = 0u; y < quad_dimensions.y(); ++y)
		{
			usize indices[2];

			for (auto x = 0u; x < vert_dimensions.x(); ++x)
			{
				indices[0] = y * vert_dimensions.x() + x;
				indices[1] = indices[0] + vert_dimensions.x();

				for (auto i : indices)
				{
					auto position = normalized_int16(vertex_positions[i]);

					std::memcpy(v, &position[0], sizeof(i16) * 3);
					v += sizeof(i16) * 3;
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
				std::memcpy(v, &position[0], sizeof(i16) * 3);
				v += sizeof(int16_t) * 3;
				std::memcpy(v, &vertex_normals[indices[1]], sizeof(float) * 3);
				v += sizeof(float) * 3;

				indices[0] = (y + 1) * vert_dimensions.x();

				position = normalized_int16(vertex_positions[indices[0]]);
				std::memcpy(v, &position[0], sizeof(i16) * 3);
				v += sizeof(int16_t) * 3;
				std::memcpy(v, &vertex_normals[indices[0]], sizeof(float) * 3);
				v += sizeof(float) * 3;
			}
		}

		// Construct VBO
		auto& vbo = model->get_vertex_buffer();
		vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data);
		model->set_vertex_offset(0);
		model->set_vertex_stride(vertex_stride);

		// Create material group
		model->get_groups().resize(1);
		render::model_group& model_group = model->get_groups().front();

		model_group.id = {};
		model_group.primitive_topology = gl::primitive_topology::triangle_strip;
		model_group.first_vertex = 0;
		model_group.vertex_count = static_cast<u32>(vertex_count);
		model_group.material_index = 0;

		return model;
	}
}

entity::id generate_terrain(entity::registry& registry, std::shared_ptr<gl::image_2d> heightmap, const math::uvec2& subdivisions, const math::transform<float>& transform, std::shared_ptr<render::material> material)
{
	if (!heightmap)
	{
		debug::log_error("Failed to generate terrain from null heightmap");
		throw std::invalid_argument("Failed to generate terrain from null heightmap");
	}

	const auto& heightmap_dimensions = heightmap->get_dimensions();
	if (heightmap_dimensions[0] < 2 || heightmap_dimensions[1] < 2)
	{
		debug::log_error("Heightmap size less than 2x2");
		throw std::runtime_error("Heightmap size less than 2x2");
	}

	if (((heightmap_dimensions[0] - 1) % (subdivisions.x() + 1)) != 0 ||
		((heightmap_dimensions[1] - 1) % (subdivisions.y() + 1)) != 0)
	{
		debug::log_error("{}x{} heightmap cannot be subdivided {}x{} times",
			heightmap_dimensions[0],
			heightmap_dimensions[1],
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
	const auto cell_quad_dimensions = math::uvec2{static_cast<unsigned int>(heightmap_dimensions[0] - 1) / grid.dimensions.x(), static_cast<unsigned int>(heightmap_dimensions[1] - 1) / grid.dimensions.y()};
	const auto cell_vert_dimensions = cell_quad_dimensions + 1u;

	const auto max_scale = math::max_element(transform.scale);
	const auto scale_ratio = transform.scale / max_scale;
	const auto vertex_scale = scale_ratio * math::fvec3{2.0f / static_cast<float>(cell_quad_dimensions.x()), 2.0f, 2.0f / static_cast<float>(cell_quad_dimensions.y())};
	const auto vertex_translation = -scale_ratio;

	// Allocate heightmap data buffer and select heightmap sampling function according to image format
	std::vector<float> heightmap_data(heightmap_dimensions[0] * heightmap_dimensions[1]);
	std::function<float(const math::uvec2&)> sample = [&](const math::uvec2& p)
		{
			return heightmap_data[p.y() * heightmap_dimensions[0] + p.x()];
		};

	// Read heightmap pixel data into buffer
	heightmap->read
	(
		0,
		0,
		0,
		0,
		heightmap_dimensions[0],
		heightmap_dimensions[1],
		1,
		gl::format::r32_sfloat,
		std::as_writable_bytes(std::span{heightmap_data})
	);

	// Generate terrain cell meshes
	for (auto cell_eid : grid.cells)
	{
		const auto& cell = registry.get<terrain_cell_component>(cell_eid);

		// Allocate cell mesh and attributes
		auto mesh = std::make_shared<brep::mesh>();
		auto& vertex_positions = static_cast<brep::attribute<math::fvec3>&>(*mesh->vertices().attributes().emplace<math::fvec3>("position"));

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
				float height = sample(pixel_position);

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

				brep::vertex* abc[3] = {a, b, c};
				brep::vertex* cbd[3] = {c, b, d};

				mesh->faces().emplace_back(abc);
				mesh->faces().emplace_back(cbd);
			}
		}

		// Generate vertex normals
		auto& vertex_normals = static_cast<brep::attribute<math::fvec3>&>(*mesh->vertices().attributes().try_emplace<math::fvec3>("normal").first);
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

				auto& normal_c = vertex_normals[index_c];
				normal_c = math::normalize(math::fvec3{(height_w - height_e) / vertex_scale.x(), 2.0f, (height_s - height_n) / vertex_scale.z()});
			}
		}

		// Construct terrain cell rigid body
		auto rigid_body = std::make_unique<physics::rigid_body>();
		rigid_body->set_mass(0.0f);
		rigid_body->set_collider(std::make_shared<physics::mesh_collider>(mesh));
		rigid_body->set_transform({transform.translation, transform.rotation, math::fvec3{max_scale, max_scale, max_scale} *0.5f});
		registry.emplace<rigid_body_component>(cell_eid, std::move(rigid_body));

		auto model = generate_terrain_model(*mesh, material, cell_quad_dimensions);
		scene_object_component scene;
		scene.object = std::make_shared<scene::static_mesh>(std::move(model));
		scene.layer_mask = 1;
		registry.emplace<scene_object_component>(cell_eid, std::move(scene));
	}

	registry.emplace<terrain_grid_component>(grid_eid, std::move(grid));
	return grid_eid;
}
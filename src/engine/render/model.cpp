// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <nlohmann/json.hpp>
#include <engine/render/model.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/debug/log.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/resource-manager.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/utility/json.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/math/constants.hpp>
#include <engine/geom/brep/mesh.hpp>
#include <functional>

using namespace engine::geom;

namespace engine::render
{
	model::model(std::shared_ptr<brep::mesh> mesh):
		m_mesh(std::move(mesh))
	{
		rebuild();
	}

	void model::rebuild()
	{
		if (!m_mesh)
		{
			m_vertex_array.reset();
			m_vertex_buffer.reset();
			m_vertex_offset = 0;
			m_vertex_stride = 0;
			m_bounds = {};
			m_groups.clear();
			return;
		}

		// Find vertex positions
		const brep::attribute<math::fvec3>* vertex_positions = nullptr;
		if (auto it = m_mesh->vertices().attributes().find("position"); it != m_mesh->vertices().attributes().end())
		{
			vertex_positions = &static_cast<const brep::attribute<math::fvec3>&>(*it);
		}
	
		// Find loop normals
		const brep::attribute<math::fvec3>* loop_normals = nullptr;
		if (auto it = m_mesh->loops().attributes().find("normal"); it != m_mesh->loops().attributes().end())
		{
			loop_normals = &static_cast<const brep::attribute<math::fvec3>&>(*it);
		}

		// Find vertex normals
		const brep::attribute<math::fvec3>* vertex_normals = nullptr;
		if (!loop_normals)
		{
			if (auto it = m_mesh->vertices().attributes().find("normal"); it != m_mesh->vertices().attributes().end())
			{
				vertex_normals = &static_cast<const brep::attribute<math::fvec3>&>(*it);
			}
		}

		// Find face normals
		const brep::attribute<math::fvec3>* face_normals = nullptr;
		if (!loop_normals && !vertex_normals)
		{
			if (auto it = m_mesh->faces().attributes().find("normal"); it != m_mesh->faces().attributes().end())
			{
				face_normals = &static_cast<const brep::attribute<math::fvec3>&>(*it);
			}
		}

		// Find loop UVs
		const brep::attribute<math::fvec2>* loop_uvs = nullptr;
		if (auto it = m_mesh->loops().attributes().find("uv"); it != m_mesh->loops().attributes().end())
		{
			loop_uvs = &static_cast<const brep::attribute<math::fvec2>&>(*it);
		}

		// Find vertex UVs
		const brep::attribute<math::fvec2>* vertex_uvs = nullptr;
		if (!loop_uvs)
		{
			if (auto it = m_mesh->vertices().attributes().find("uv"); it != m_mesh->vertices().attributes().end())
			{
				vertex_uvs = &static_cast<const brep::attribute<math::fvec2>&>(*it);
			}
		}

		// Find loop tangents
		const brep::attribute<math::fvec4>* loop_tangents = nullptr;
		if (auto it = m_mesh->loops().attributes().find("tangent"); it != m_mesh->loops().attributes().end())
		{
			loop_tangents = &static_cast<const brep::attribute<math::fvec4>&>(*it);
		}

		// Find vertex tangents
		const brep::attribute<math::fvec4>* vertex_tangents = nullptr;
		if (!loop_tangents)
		{
			if (auto it = m_mesh->vertices().attributes().find("tangent"); it != m_mesh->vertices().attributes().end())
			{
				vertex_tangents = &static_cast<const brep::attribute<math::fvec4>&>(*it);
			}
		}

		// Find vertex bone indices
		const brep::attribute<math::vector<u16, 4>>* vertex_bone_indices = nullptr;
		if (auto it = m_mesh->vertices().attributes().find("bone_indices"); it != m_mesh->vertices().attributes().end())
		{
			vertex_bone_indices = &static_cast<const brep::attribute<math::vector<u16, 4>>&>(*it);
		}

		// Find vertex bone weights
		const brep::attribute<math::fvec4>* vertex_bone_weights = nullptr;
		if (auto it = m_mesh->vertices().attributes().find("bone_weights"); it != m_mesh->vertices().attributes().end())
		{
			vertex_bone_weights = &static_cast<const brep::attribute<math::fvec4>&>(*it);
		}

		// Find loop colors
		const brep::attribute<math::fvec4>* loop_colors = nullptr;
		if (auto it = m_mesh->loops().attributes().find("color"); it != m_mesh->loops().attributes().end())
		{
			loop_colors = &static_cast<const brep::attribute<math::fvec4>&>(*it);
		}

		// Find vertex colors
		const brep::attribute<math::fvec4>* vertex_colors = nullptr;
		if (!loop_colors)
		{
			if (auto it = m_mesh->vertices().attributes().find("color"); it != m_mesh->vertices().attributes().end())
			{
				vertex_colors = &static_cast<const brep::attribute<math::fvec4>&>(*it);
			}
		}

		// Find face materials
		const brep::attribute<u8>* face_materials = nullptr;
		if (auto it = m_mesh->faces().attributes().find("material"); it != m_mesh->faces().attributes().end())
		{
			face_materials = &static_cast<const brep::attribute<u8>&>(*it);
		}
	
		// Allocate model
		auto model = std::make_unique<render::model>();
	
		// Construct model VAO
		usize vertex_stride = 0;
		std::vector<gl::vertex_input_attribute> vertex_attributes;

		// Vertex interleaving functions
		std::vector<std::function<usize(std::byte*&, brep::loop*)>> interleavers;

		// Positions
		if (vertex_positions)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::position;
			attribute.binding = 0;
			attribute.format = gl::format::r32g32b32_sfloat;
			attribute.offset = 0;

			interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
			{
				const auto& position = (*vertex_positions)[loop->vertex()->index()];
				std::memcpy(data, position.data(), sizeof(position));
				return sizeof(position);
			});
		
			vertex_stride += 3 * sizeof(float);
		}

		// Normals
		if (loop_normals || vertex_normals || face_normals)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::normal;
			attribute.binding = 0;
			attribute.format = gl::format::r32g32b32_sfloat;
			attribute.offset = static_cast<u32>(vertex_stride);

			if (loop_normals)
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& normal = (*loop_normals)[loop->index()];
					std::memcpy(data, normal.data(), sizeof(normal));
					return sizeof(normal);
				});
			}
			else if (vertex_normals)
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& normal = (*vertex_normals)[loop->vertex()->index()];
					std::memcpy(data, normal.data(), sizeof(normal));
					return sizeof(normal);
				});
			}
			else
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& normal = (*face_normals)[loop->face()->index()];
					std::memcpy(data, normal.data(), sizeof(normal));
					return sizeof(normal);
				});
			}
		
			vertex_stride += 3 * sizeof(float);
		}

		// UVs
		if (loop_uvs || vertex_uvs)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::uv;
			attribute.binding = 0;
			attribute.format = gl::format::r32g32_sfloat;
			attribute.offset = static_cast<u32>(vertex_stride);

			if (loop_uvs)
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& uv = (*loop_uvs)[loop->index()];
					std::memcpy(data, uv.data(), sizeof(uv));
					return sizeof(uv);
				});
			}
			else
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& uv = (*vertex_uvs)[loop->vertex()->index()];
					std::memcpy(data, uv.data(), sizeof(uv));
					return sizeof(uv);
				});
			}
		
			vertex_stride += 2 * sizeof(float);
		}

		// Tangents
		if (loop_tangents || vertex_tangents)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::tangent;
			attribute.binding = 0;
			attribute.format = gl::format::r32g32b32a32_sfloat;
			attribute.offset = static_cast<u32>(vertex_stride);

			if (loop_tangents)
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& tangent = (*loop_tangents)[loop->index()];
					std::memcpy(data, tangent.data(), sizeof(tangent));
					return sizeof(tangent);
				});
			}
			else
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& tangent = (*vertex_tangents)[loop->vertex()->index()];
					std::memcpy(data, tangent.data(), sizeof(tangent));
					return sizeof(tangent);
				});
			}
		
			vertex_stride += 4 * sizeof(float);
		}

		// Bone indices
		if (vertex_bone_indices)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::bone_index;
			attribute.binding = 0;
			attribute.format = gl::format::r16g16b16a16_uint;
			attribute.offset = static_cast<u32>(vertex_stride);

			interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
			{
				const auto& bone_indices = (*vertex_bone_indices)[loop->vertex()->index()];
				std::memcpy(data, bone_indices.data(), sizeof(bone_indices));
				return sizeof(bone_indices);
			});
		
			vertex_stride += 4 * sizeof(u16);
		}

		// Bone weights
		if (vertex_bone_weights)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::bone_weight;
			attribute.binding = 0;
			attribute.format = gl::format::r32g32b32a32_sfloat;
			attribute.offset = static_cast<u32>(vertex_stride);

			interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
			{
				const auto& bone_weights = (*vertex_bone_weights)[loop->vertex()->index()];
				std::memcpy(data, bone_weights.data(), sizeof(bone_weights));
				return sizeof(bone_weights);
			});
		
			vertex_stride += 4 * sizeof(float);
		}

		// Colors
		if (loop_colors || vertex_colors)
		{
			auto& attribute = vertex_attributes.emplace_back();
			attribute.location = render::vertex_attribute_location::color;
			attribute.binding = 0;
			attribute.format = gl::format::r32g32b32a32_sfloat;
			attribute.offset = static_cast<u32>(vertex_stride);

			if (loop_colors)
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& color = (*loop_colors)[loop->index()];
					std::memcpy(data, color.data(), sizeof(color));
					return sizeof(color);
				});
			}
			else
			{
				interleavers.emplace_back([&](std::byte* data, brep::loop* loop)
				{
					const auto& color = (*vertex_colors)[loop->vertex()->index()];
					std::memcpy(data, color.data(), sizeof(color));
					return sizeof(color);
				});
			}
		
			vertex_stride += 4 * sizeof(float);
		}

		// Allocate interleaved vertex data buffer
		std::vector<std::byte> vertex_data(m_mesh->faces().size() * 3 * vertex_stride);
	
		// Interleave vertex data
		{
			std::byte* data = vertex_data.data();
			for (auto face: m_mesh->faces())
			{
				for (auto loop: face->loops())
				{
					for (const auto& interleaver: interleavers)
					{
						data += interleaver(data, loop);
					}
				}
			}
		}

		// Construct VAO
		m_vertex_array = std::make_shared<gl::vertex_array>(vertex_attributes);

		// Construct VBO
		m_vertex_buffer = std::make_shared<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data);
		m_vertex_offset = 0;
		m_vertex_stride = vertex_stride;

		// Calculate model bounds
		get_bounds() = {math::inf<math::fvec3>, -math::inf<math::fvec3>};
		if (vertex_positions)
		{
			for (const auto& position: *vertex_positions)
			{
				get_bounds().extend(position);
			}
		}

		// Construct material groups
		m_groups.clear();
		if (face_materials)
		{
			model_group group;
			group.id = {};
			group.primitive_topology = gl::primitive_topology::triangle_list;
			group.first_vertex = 0;
			group.vertex_count = 0;
			group.material_index = 0;

			for (auto face: m_mesh->faces())
			{
				const auto face_material_index = static_cast<u32>((*face_materials)[face->index()]);

				if (face_material_index != group.material_index)
				{
					if (face_material_index < group.material_index)
					{
						throw std::runtime_error("Model mesh faces are not sorted by material");
					}

					if (group.vertex_count)
					{
						m_groups.emplace_back(group);
					}

					group.first_vertex = static_cast<u32>(face->index() * 3);
					group.vertex_count = 0;
					group.material_index = face_material_index;
				}

				group.vertex_count += 3;
			}

			m_groups.emplace_back(group);
		}
		else
		{
			auto& default_group = m_groups.emplace_back();
			default_group.id = {};
			default_group.primitive_topology = gl::primitive_topology::triangle_list;
			default_group.first_vertex = 0;
			default_group.vertex_count = static_cast<u32>(m_mesh->faces().size() * 3);
			default_group.material_index = 0;
		}
	}
}

namespace engine::resources
{
	template <>
	std::unique_ptr<render::model> resource_loader<render::model>::load(resources::resource_manager& resource_manager, std::shared_ptr<deserialize_context> ctx)
	{
		// Read file into buffer
		std::string file_buffer(ctx->size(), '\0');
		ctx->read8(reinterpret_cast<std::byte*>(file_buffer.data()), ctx->size());

		// Parse JSON from file buffer
		const auto json = nlohmann::json::parse(file_buffer, nullptr, true, true);

		// Check version string
		const auto& version = json.at("version").get_ref<const std::string&>();
		if (version != "1.0.0")
		{
			throw deserialize_error(std::format("Unsupported model format (version {})", version));
		}

		// Load mesh
		const auto& mesh_path = json.at("mesh").get_ref<const std::string&>();
		auto mesh = resource_manager.load<brep::mesh>(mesh_path);
		if (!mesh)
		{
			auto error_message = std::format("Failed to load model mesh \"{}\"", mesh_path);
			log_error("{}", error_message);
			throw deserialize_error(std::move(error_message));
		}

		// Load materials
		std::vector<std::shared_ptr<render::material>> materials;
		if (auto materials_element = json.find("materials"); materials_element != json.end())
		{
			for (const auto& material_element : *materials_element)
			{
				if (material_element.is_null())
				{
					materials.emplace_back(nullptr);
				}
				else
				{
					const auto& material_path = material_element.get_ref<const std::string&>();

					materials.emplace_back(resource_manager.load<render::material>(material_path));
					if (!materials.back())
					{
						log_error("Failed to load model material \"{}\".", material_path);
					}
				}
			}
		}

		// Load skeleton
		std::shared_ptr<animation::skeleton> skeleton;
		if (auto skeleton_element = json.find("skeleton"); skeleton_element != json.end())
		{
			if (!skeleton_element->is_null())
			{
				const auto& skeleton_path = skeleton_element->get_ref<const std::string&>();
				if (!skeleton_path.empty())
				{
					skeleton = resource_manager.load<animation::skeleton>(skeleton_path);
					if (!skeleton)
					{
						log_error("Failed to load model skeleton \"{}\"", skeleton_path);
					}
				}
			}
		}

		// Construct model
		auto model = std::make_unique<render::model>(mesh);
		model->materials() = std::move(materials);
		model->skeleton() = std::move(skeleton);

		return model;
	}
}

// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <nlohmann/json.hpp>
import engine.geom.brep.mesh;
import engine.resources.deserializer;
import engine.resources.deserialize_error;
import engine.resources.resource_loader;
import engine.debug.log;
import engine.utility.json;
import engine.math.vector;
import engine.utility.sized_types;
import <format>;

namespace engine::geom::brep
{
	mesh::mesh() noexcept
	{
		m_vertices.set_mesh(this);
		m_edges.set_mesh(this);
		m_loops.set_mesh(this);
		m_faces.set_mesh(this);
	}

	mesh::mesh(const mesh& other):
		mesh()
	{
		*this = other;
	}

	mesh& mesh::operator=(const mesh& other)
	{
		// Allocate containers
		m_vertices.m_elements.resize(other.m_vertices.m_elements.size());
		m_edges.m_elements.resize(other.m_edges.m_elements.size());
		m_loops.m_elements.resize(other.m_loops.m_elements.size());
		m_faces.m_elements.resize(other.m_faces.m_elements.size());

		// Copy-construct elements
		for (usize i = 0; i < m_vertices.m_elements.size(); ++i)
		{
			m_vertices.m_elements[i] = std::make_unique<vertex>(*other.m_vertices.m_elements[i]);
		}
		for (usize i = 0; i < m_edges.m_elements.size(); ++i)
		{
			m_edges.m_elements[i] = std::make_unique<edge>(*other.m_edges.m_elements[i]);
		}
		for (usize i = 0; i < m_loops.m_elements.size(); ++i)
		{
			m_loops.m_elements[i] = std::make_unique<loop>(*other.m_loops.m_elements[i]);
		}
		for (usize i = 0; i < m_faces.m_elements.size(); ++i)
		{
			m_faces.m_elements[i] = std::make_unique<face>(*other.m_faces.m_elements[i]);
		}

		// Copy per-element attributes
		m_vertices.m_attribute_map = other.m_vertices.m_attribute_map;
		m_edges.m_attribute_map = other.m_edges.m_attribute_map;
		m_loops.m_attribute_map = other.m_loops.m_attribute_map;
		m_faces.m_attribute_map = other.m_faces.m_attribute_map;

		// Reassign element pointers
		for (const auto& vertex : m_vertices.m_elements)
		{
			if (!vertex->edges().empty())
			{
				vertex->m_edges.m_head = m_edges.m_elements[vertex->m_edges.m_head->m_index].get();
			}
		}
		for (const auto& edge : m_edges.m_elements)
		{
			edge->m_vertices[0] = m_vertices.m_elements[edge->m_vertices[0]->m_index].get();
			edge->m_vertices[1] = m_vertices.m_elements[edge->m_vertices[1]->m_index].get();
			edge->m_vertex_next[0] = m_edges.m_elements[edge->m_vertex_next[0]->m_index].get();
			edge->m_vertex_next[1] = m_edges.m_elements[edge->m_vertex_next[1]->m_index].get();
			edge->m_vertex_previous[0] = m_edges.m_elements[edge->m_vertex_previous[0]->m_index].get();
			edge->m_vertex_previous[1] = m_edges.m_elements[edge->m_vertex_previous[1]->m_index].get();

			if (!edge->loops().empty())
			{
				edge->m_loops.m_head = m_loops.m_elements[edge->m_loops.m_head->m_index].get();
			}
		}
		for (const auto& loop : m_loops.m_elements)
		{
			loop->m_vertex = m_vertices.m_elements[loop->m_vertex->m_index].get();
			loop->m_edge = m_edges.m_elements[loop->m_edge->m_index].get();
			loop->m_face = m_faces.m_elements[loop->m_face->m_index].get();
			loop->m_edge_next = m_loops.m_elements[loop->m_edge_next->m_index].get();
			loop->m_edge_previous = m_loops.m_elements[loop->m_edge_previous->m_index].get();
			loop->m_face_next = m_loops.m_elements[loop->m_face_next->m_index].get();
			loop->m_face_previous = m_loops.m_elements[loop->m_face_previous->m_index].get();
		}
		for (const auto& face : m_faces.m_elements)
		{
			face->m_loops.m_head = m_loops.m_elements[face->m_loops.m_head->m_index].get();
		}

		return *this;
	}

	void mesh::clear() noexcept
	{
		m_vertices.clear();
	}
}

namespace engine::resources
{
	using namespace engine::geom;

	template <class T>
	void make_attribute(brep::attribute_map& attribute_map, const std::string& attribute_name, usize domain_size, const auto& data_element)
	{
		const usize vector_size = data_element.size() / domain_size;

		T* attribute_data = nullptr;
		if (vector_size == 1)
		{
			attribute_data = static_cast<brep::attribute<T>&>(*attribute_map.emplace<T>(attribute_name)).data();
		}
		else if (vector_size == 2)
		{
			attribute_data = static_cast<brep::attribute<math::vector<T, 2>>&>(*attribute_map.emplace<math::vector<T, 2>>(attribute_name)).data()->data();
		}
		else if (vector_size == 3)
		{
			attribute_data = static_cast<brep::attribute<math::vector<T, 3>>&>(*attribute_map.emplace<math::vector<T, 3>>(attribute_name)).data()->data();
		}
		else if (vector_size == 4)
		{
			attribute_data = static_cast<brep::attribute<math::vector<T, 4>>&>(*attribute_map.emplace<math::vector<T, 4>>(attribute_name)).data()->data();
		}
		else
		{
			throw deserialize_error(std::format("B-rep mesh attribute type has unsupported vector size ({}).", vector_size));
		}

		for (usize i = 0; i < data_element.size(); ++i)
		{
			*attribute_data = data_element[i].template get<T>();
			++attribute_data;
		}
	}

	/// Deserializes a mesh.
	/// @param[out] mesh Mesh to deserialize.
	/// @param[in,out] ctx Deserialize context.
	/// @throw deserialize_error Read error.
	template <>
	void deserializer<brep::mesh>::deserialize(brep::mesh& mesh, deserialize_context& ctx)
	{
		// Read file into buffer
		std::vector<std::byte> file_buffer(ctx.size());
		ctx.read8(file_buffer.data(), ctx.size());

		// Parse msgpack from file buffer
		const auto msgpack = nlohmann::json::from_msgpack(file_buffer.begin(), file_buffer.end());

		// Check version string
		const auto& version = msgpack.at("version").get_ref<const std::string&>();
		if (version != "1.0.0")
		{
			throw deserialize_error(std::format("Unsupported mesh format (version {}).", version));
		}

		// Validate edge data
		const auto& edges_element = msgpack.at("edges");
		if (edges_element.size() % 2 != 0)
		{
			throw deserialize_error("B-rep mesh edge data is missing a vertex.");
		}

		// Make vertices
		const auto vertex_count = msgpack.at("vertices").get<usize>();
		for (usize i = 0; i < vertex_count; ++i)
		{
			mesh.vertices().emplace_back();
		}

		// Make edges
		for (usize i = 0; i < edges_element.size(); i += 2)
		{
			const auto vertex_a = mesh.vertices()[edges_element[i].get<usize>()];
			const auto vertex_b = mesh.vertices()[edges_element[i + 1].get<usize>()];
			mesh.edges().emplace_back(vertex_a, vertex_b);
		}

		// Make faces
		for (const auto& face_element : msgpack.at("faces"))
		{
			std::vector<brep::vertex*> vertices(face_element.size(), nullptr);
			for (usize i = 0; i < face_element.size(); ++i)
			{
				vertices[i] = mesh.vertices()[face_element[i].get<usize>()];
			}

			mesh.faces().emplace_back(vertices);
		}

		// Attributes
		if (auto attributes_element = msgpack.find("attributes"); attributes_element != msgpack.end())
		{
			for (const auto& [attribute_name, attribute_element] : attributes_element->items())
			{
				const auto& attribute_type = attribute_element.at("type").get_ref<const std::string&>();
				const auto& attribute_domain = attribute_element.at("domain").get_ref<const std::string&>();
				const auto& attribute_data_element = attribute_element.at("data");

				usize domain_size = 0;
				brep::attribute_map* domain_attribute_map = nullptr;
				if (attribute_domain == "vertex")
				{
					domain_size = mesh.vertices().size();
					domain_attribute_map = &mesh.vertices().attributes();
				}
				else if (attribute_domain == "edge")
				{
					domain_size = mesh.edges().size();
					domain_attribute_map = &mesh.edges().attributes();
				}
				else if (attribute_domain == "loop")
				{
					domain_size = mesh.loops().size();
					domain_attribute_map = &mesh.loops().attributes();
				}
				else if (attribute_domain == "face")
				{
					domain_size = mesh.faces().size();
					domain_attribute_map = &mesh.faces().attributes();
				}
				else
				{
					throw deserialize_error(std::format("B-rep mesh attribute has unsupported domain (\"{}\").", attribute_domain));
				}

				if (attribute_type == "float64")
				{
					make_attribute<double>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "float32")
				{
					make_attribute<float>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "uint64")
				{
					make_attribute<u64>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "uint32")
				{
					make_attribute<u32>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "uint16")
				{
					make_attribute<u16>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "uint8")
				{
					make_attribute<u8>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "int64")
				{
					make_attribute<i64>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "int32")
				{
					make_attribute<i32>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "int16")
				{
					make_attribute<i16>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else if (attribute_type == "int8")
				{
					make_attribute<i8>(*domain_attribute_map, attribute_name, domain_size, attribute_data_element);
				}
				else
				{
					throw deserialize_error(std::format("B-rep mesh attribute has unsupported type (\"{}\").", attribute_type));
				}
			}
		}
	}

	template <>
	std::unique_ptr<brep::mesh> resource_loader<brep::mesh>::load(resource_manager&, std::shared_ptr<deserialize_context> ctx)
	{
		auto resource = std::make_unique<brep::mesh>();
		deserializer<brep::mesh>().deserialize(*resource, *ctx);
		return resource;
	}
}

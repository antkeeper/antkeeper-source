// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/mesh.hpp>
#include <engine/resources/deserializer.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserialize.hpp>
#include <engine/debug/log.hpp>
#include <engine/math/vector.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/utility/version.hpp>
#include <format>

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
	namespace
	{
		enum class attribute_type: u8
		{
			unknown,
			i8,
			i16,
			i32,
			i64,
			u8,
			u16,
			u32,
			u64,
			f32,
			f64
		};

		enum class attribute_domain : u8
		{
			unknown,
			vertex,
			edge,
			loop,
			face
		};
	}

	using namespace engine::geom;

	template <class T>
	void make_attribute(std::span<const std::byte>& stream, brep::attribute_map& attribute_map, const std::string& attribute_name, usize domain_size, u8 vector_size)
	{
		T* attribute_data = nullptr;

		switch (vector_size)
		{
			case 1:
				attribute_data = static_cast<brep::attribute<T>&>(*attribute_map.emplace<T>(attribute_name)).data();
				break;
			case 2:
				attribute_data = static_cast<brep::attribute<math::vec2<T>>&>(*attribute_map.emplace<math::vec2<T>>(attribute_name)).data()->data();
				break;
			case 3:
				attribute_data = static_cast<brep::attribute<math::vec3<T>>&>(*attribute_map.emplace<math::vec3<T>>(attribute_name)).data()->data();
				break;
			case 4:
				attribute_data = static_cast<brep::attribute<math::vec4<T>>&>(*attribute_map.emplace<math::vec4<T>>(attribute_name)).data()->data();
				break;
			default:
				throw deserialize_error(std::format("B-rep mesh attribute type has unsupported vector size ({}).", vector_size));
		}

		std::memcpy(attribute_data, stream.data(), domain_size * vector_size * sizeof(T));
		if constexpr (std::endian::native != std::endian::little)
		{
			for (usize i = 0; i < domain_size * vector_size; ++i)
			{
				if constexpr (sizeof(T) == sizeof(u16))
				{
					attribute_data[i] = std::bit_cast<T>(std::byteswap(std::bit_cast<u16>(attribute_data[i])));
				}
				else if constexpr (sizeof(T) == sizeof(u32))
				{
					attribute_data[i] = std::bit_cast<T>(std::byteswap(std::bit_cast<u32>(attribute_data[i])));
				}
				else if constexpr (sizeof(T) == sizeof(u64))
				{
					attribute_data[i] = std::bit_cast<T>(std::byteswap(std::bit_cast<u64>(attribute_data[i])));
				}
			}
		}

		stream = stream.subspan(domain_size * vector_size * sizeof(T));
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

		std::span<const std::byte> stream = file_buffer;

		// Check file format version
		u32 packed_version = 0;
		deserialize_le(stream, packed_version);
		version unpacked_version{(packed_version >> 16) & 255, (packed_version >> 8) & 255, packed_version & 255};
		if (unpacked_version != version{1, 0, 0})
		{
			throw deserialize_error(std::format("Unsupported mesh format (version {}).", unpacked_version));
		}

		// Read mesh name length
		u16 mesh_name_length = 0;
		deserialize_le(stream, mesh_name_length);

		// Read mesh name
		std::string mesh_name;
		if (mesh_name_length > 0)
		{
			mesh_name.resize(mesh_name_length);
			std::memcpy(mesh_name.data(), stream.data(), mesh_name_length);
			stream = stream.subspan(mesh_name_length);
		}

		// Read vertex count
		u32 vertex_count = 0;
		deserialize_le(stream, vertex_count);

		// Read edge count
		u32 edge_count = 0;
		deserialize_le(stream, edge_count);

		// Make vertices
		for (u32 i = 0; i < vertex_count; ++i)
		{
			mesh.vertices().emplace_back();
		}

		// Make edges
		for (u32 i = 0; i < edge_count; ++i)
		{
			u32 vertex_index_a = 0;
			u32 vertex_index_b = 0;

			deserialize_le(stream, vertex_index_a);
			deserialize_le(stream, vertex_index_b);

			if (vertex_index_a >= vertex_count || vertex_index_b >= vertex_count)
			{
				throw deserialize_error("B-rep mesh edge data has invalid vertex index.");
			}

			mesh.edges().emplace_back(mesh.vertices()[vertex_index_a], mesh.vertices()[vertex_index_b]);
		}

		// Read face count
		u32 face_count = 0;
		deserialize_le(stream, face_count);

		// Make faces
		std::vector<brep::vertex*> face_vertices;
		for (u32 i = 0; i < face_count; ++i)
		{
			u32 loop_count = 0;
			deserialize_le(stream, loop_count);

			if (loop_count < 3)
			{
				throw deserialize_error("B-rep mesh face data has invalid loop count.");
			}

			face_vertices.resize(loop_count);
			for (u32 j = 0; j < loop_count; ++j)
			{
				u32 vertex_index = 0;
				deserialize_le(stream, vertex_index);
				if (vertex_index >= vertex_count)
				{
					throw deserialize_error("B-rep mesh face data has invalid vertex index.");
				}

				face_vertices[j] = mesh.vertices()[vertex_index];
			}

			mesh.faces().emplace_back(face_vertices);
		}

		// Read attribute count
		u32 attribute_count = 0;
		deserialize_le(stream, attribute_count);

		for (u32 i = 0; i < attribute_count; ++i)
		{
			// Read attribute name length
			u16 attribute_name_length = 0;
			deserialize_le(stream, attribute_name_length);

			// Read attribute name
			std::string attribute_name;
			if (attribute_name_length > 0)
			{
				attribute_name.resize(attribute_name_length);
				std::memcpy(attribute_name.data(), stream.data(), attribute_name_length);
				stream = stream.subspan(attribute_name_length);
			}

			// Read attribute domain
			attribute_domain domain = attribute_domain::unknown;
			deserialize_le(stream, reinterpret_cast<u8&>(domain));

			// Read attribute type
			attribute_type type = attribute_type::unknown;
			deserialize_le(stream, reinterpret_cast<u8&>(type));

			// Read attribute vector size
			u8 vector_size = 0;
			deserialize_le(stream, vector_size);

			usize domain_size = 0;
			brep::attribute_map* domain_attribute_map = nullptr;

			switch (domain)
			{
				case attribute_domain::vertex:
					domain_size = mesh.vertices().size();
					domain_attribute_map = &mesh.vertices().attributes();
					break;
				case attribute_domain::edge:
					domain_size = mesh.edges().size();
					domain_attribute_map = &mesh.edges().attributes();
					break;
				case attribute_domain::loop:
					domain_size = mesh.loops().size();
					domain_attribute_map = &mesh.loops().attributes();
					break;
				case attribute_domain::face:
					domain_size = mesh.faces().size();
					domain_attribute_map = &mesh.faces().attributes();
					break;
				case attribute_domain::unknown:
				default:
					throw deserialize_error(std::format("B-rep mesh attribute has unsupported domain ({}).", static_cast<u8>(domain)));
			}

			switch (type)
			{
				case attribute_type::i8:
					make_attribute<i8>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::i16:
					make_attribute<i16>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::i32:
					make_attribute<i32>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::i64:
					make_attribute<i64>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::u8:
					make_attribute<u8>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::u16:
					make_attribute<u16>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::u32:
					make_attribute<u32>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::u64:
					make_attribute<u64>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::f32:
					make_attribute<f32>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::f64:
					make_attribute<f64>(stream, *domain_attribute_map, attribute_name, domain_size, vector_size);
					break;
				case attribute_type::unknown:
				default:
					throw deserialize_error(std::format("B-rep mesh attribute has unsupported type ({}).", static_cast<u8>(type)));
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

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-mesh.hpp>
#include <engine/math/vector.hpp>
#include <engine/resources/resource-loader.hpp>
#include <engine/resources/deserialize-error.hpp>
#include <engine/resources/deserializer.hpp>
#include <array>

namespace geom {

brep_mesh::brep_mesh() noexcept
{
	m_vertices.set_mesh(this);
	m_edges.set_mesh(this);
	m_loops.set_mesh(this);
	m_faces.set_mesh(this);
}

brep_mesh::brep_mesh(const brep_mesh& other):
	brep_mesh()
{
	*this = other;
}

brep_mesh& brep_mesh::operator=(const brep_mesh& other)
{
	// Allocate containers
	m_vertices.m_elements.resize(other.m_vertices.m_elements.size());
	m_edges.m_elements.resize(other.m_edges.m_elements.size());
	m_loops.m_elements.resize(other.m_loops.m_elements.size());
	m_faces.m_elements.resize(other.m_faces.m_elements.size());
	
	// Copy-construct elements
	for (std::size_t i = 0; i < m_vertices.m_elements.size(); ++i)
	{
		m_vertices.m_elements[i] = std::make_unique<brep_vertex>(*other.m_vertices.m_elements[i]);
	}
	for (std::size_t i = 0; i < m_edges.m_elements.size(); ++i)
	{
		m_edges.m_elements[i] = std::make_unique<brep_edge>(*other.m_edges.m_elements[i]);
	}
	for (std::size_t i = 0; i < m_loops.m_elements.size(); ++i)
	{
		m_loops.m_elements[i] = std::make_unique<brep_loop>(*other.m_loops.m_elements[i]);
	}
	for (std::size_t i = 0; i < m_faces.m_elements.size(); ++i)
	{
		m_faces.m_elements[i] = std::make_unique<brep_face>(*other.m_faces.m_elements[i]);
	}
	
	// Copy per-element attributes
	m_vertices.m_attribute_map = other.m_vertices.m_attribute_map;
	m_edges.m_attribute_map = other.m_edges.m_attribute_map;
	m_loops.m_attribute_map = other.m_loops.m_attribute_map;
	m_faces.m_attribute_map = other.m_faces.m_attribute_map;
	
	// Reassign element pointers
	for (const auto& vertex: m_vertices.m_elements)
	{
		if (!vertex->edges().empty())
		{
			vertex->m_edges.m_head = m_edges.m_elements[vertex->m_edges.m_head->m_index].get();
		}
	}
	for (const auto& edge: m_edges.m_elements)
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
	for (const auto& loop: m_loops.m_elements)
	{
		loop->m_vertex = m_vertices.m_elements[loop->m_vertex->m_index].get();
		loop->m_edge = m_edges.m_elements[loop->m_edge->m_index].get();
		loop->m_face = m_faces.m_elements[loop->m_face->m_index].get();
		loop->m_edge_next = m_loops.m_elements[loop->m_edge_next->m_index].get();
		loop->m_edge_previous = m_loops.m_elements[loop->m_edge_previous->m_index].get();
		loop->m_face_next = m_loops.m_elements[loop->m_face_next->m_index].get();
		loop->m_face_previous = m_loops.m_elements[loop->m_face_previous->m_index].get();
	}
	for (const auto& face: m_faces.m_elements)
	{
		face->m_loops.m_head = m_loops.m_elements[face->m_loops.m_head->m_index].get();
	}
	
	return *this;
}

void brep_mesh::clear() noexcept
{
	m_vertices.clear();
}

} // namespace geom

/**
 * Deserializes a mesh.
 *
 * @param[out] mesh Mesh to deserialize.
 * @param[in,out] ctx Deserialize context.
 *
 * @throw deserialize_error Read error.
 */
template <>
void deserializer<geom::brep_mesh>::deserialize(geom::brep_mesh& mesh, deserialize_context& ctx)
{
	// Read vertex, edge, and face counts
	std::uint32_t vertex_count = 0;
	std::uint32_t edge_count = 0;
	std::uint32_t face_count = 0;
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&vertex_count), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&edge_count), 1);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(&face_count), 1);
	
	// Make vertices
	for (std::size_t i = 0; i < vertex_count; ++i)
	{
		mesh.vertices().emplace_back();
	}
	
	// Read edge vertex indices into buffer
	std::vector<std::array<std::uint32_t, 2>> edges(edge_count);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(edges.data()), edge_count * 2);
	
	// Make edges
	for (const auto& e: edges)
	{
		mesh.edges().emplace_back(mesh.vertices()[e[0]], mesh.vertices()[e[1]]);
	}
	edges.clear();
	
	// Read face vertex indices into buffer
	std::vector<std::array<std::uint32_t, 3>> faces(face_count);
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(faces.data()), face_count * 3);
	
	// Make faces
	for (const auto& f: faces)
	{
		geom::brep_vertex* vertices[3] = {mesh.vertices()[f[0]], mesh.vertices()[f[1]], mesh.vertices()[f[2]]};
		mesh.faces().emplace_back(vertices);
	}
	faces.clear();
	
	// Make vertex attributes
	auto& vertex_positions = static_cast<geom::brep_attribute<math::fvec3>&>(*mesh.vertices().attributes().emplace<math::fvec3>("position"));
	
	// Read vertex attributes
	ctx.read32<std::endian::little>(reinterpret_cast<std::byte*>(vertex_positions.data()), vertex_count * 3);
}

template <>
std::unique_ptr<geom::brep_mesh> resource_loader<geom::brep_mesh>::load([[maybe_unused]] ::resource_manager& resource_manager, deserialize_context& ctx)
{
	auto resource = std::make_unique<geom::brep_mesh>();
	deserializer<geom::brep_mesh>().deserialize(*resource, ctx);
	return resource;
}

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/brep-operations.hpp>
#include <engine/math/vector.hpp>
#include <engine/render/vertex-attribute-location.hpp>
#include <engine/debug/log.hpp>
#include <algorithm>
#include <cmath>

namespace geom {

void generate_face_normals(brep_mesh& mesh)
{
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	auto& face_normals = static_cast<brep_attribute<math::fvec3>&>(*mesh.faces().attributes().try_emplace<math::fvec3>("normal").first);
	
	for (brep_face* face: mesh.faces())
	{
		auto loop = face->loops().begin();
		const auto& a = vertex_positions[loop->vertex()->index()];
		const auto& b = vertex_positions[(++loop)->vertex()->index()];
		const auto& c = vertex_positions[(++loop)->vertex()->index()];
		
		face_normals[face->index()] = math::normalize(math::cross(b - a, c - a));
	}
}

void generate_vertex_normals(brep_mesh& mesh)
{
	// Generate face normals if they don't exist
	if (!mesh.faces().attributes().contains("normal"))
	{
		generate_face_normals(mesh);
	}
	
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	const auto& face_normals = mesh.faces().attributes().at<math::fvec3>("normal");
	auto& vertex_normals = static_cast<brep_attribute<math::fvec3>&>(*mesh.vertices().attributes().try_emplace<math::fvec3>("normal").first);
	
	for (brep_vertex* vertex: mesh.vertices())
	{
		// Zero vertex normal
		auto& vertex_normal = vertex_normals[vertex->index()];
		vertex_normal = {};
		
		// Skip vertices with no edges
		if (vertex->edges().empty())
		{
			continue;
		}
		
		// Get vertex position
		const auto& vertex_position = vertex_positions[vertex->index()];
		
		// For each edge bounded by this vertex
		for (brep_edge* edge: vertex->edges())
		{
			// Skip edges with no associated face
			if (edge->loops().empty())
			{
				continue;
			}
			
			// Calculate direction vector of current edge
			const auto direction0 = math::normalize
			(
				vertex_positions[edge->vertices()[edge->vertices().front() == vertex]->index()] -
				vertex_position
			);
			
			// For each edge loop
			for (brep_loop* loop: edge->loops())
			{
				// Skip loops not originating at vertex
				if (loop->vertex() != vertex)
				{
					continue;
				}
				
				// Calculate direction vector of previous edge
				const auto direction1 = math::normalize
				(
					vertex_positions[loop->previous()->vertex()->index()] -
					vertex_position
				);
				
				// Find angle between two edges
				const auto cos_edge_angle = math::dot(direction0, direction1);
				const auto edge_angle = std::acos(cos_edge_angle);
				
				// Weigh face normal by edge angle and add to vertex normal
				vertex_normal += face_normals[loop->face()->index()] * edge_angle;
			}
		}
		
		// Normalize vertex normal
		const auto sqr_length = math::sqr_length(vertex_normal);
		if (sqr_length)
		{
			vertex_normal /= std::sqrt(sqr_length);
		}
	}
}

void generate_loop_barycentric(brep_mesh& mesh)
{
	const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
	auto& loop_barycentric =  static_cast<brep_attribute<math::fvec3>&>(*mesh.loops().attributes().try_emplace<math::fvec3>("barycentric").first);
	
	for (brep_face* face: mesh.faces())
	{
		auto loop = face->loops().begin();
		loop_barycentric[loop->index()] = {1.0f, 0.0f, 0.0f};
		loop_barycentric[(++loop)->index()] = {0.0f, 1.0f, 0.0f};
		loop_barycentric[(++loop)->index()] = {0.0f, 0.0f, 1.0f};
	}
}

std::unique_ptr<render::model> generate_model(const brep_mesh& mesh, std::shared_ptr<render::material> material)
{
	// Get vertex positions
	const geom::brep_attribute<math::fvec3>* vertex_positions = nullptr;
	if (auto attribute_it = mesh.vertices().attributes().find("position"); attribute_it != mesh.vertices().attributes().end())
	{
		vertex_positions = &static_cast<const geom::brep_attribute<math::fvec3>&>(*attribute_it);
	}
	
	// Get vertex normals
	const geom::brep_attribute<math::fvec3>* vertex_normals = nullptr;
	if (auto attribute_it = mesh.vertices().attributes().find("normal"); attribute_it != mesh.vertices().attributes().end())
	{
		vertex_normals = &static_cast<const geom::brep_attribute<math::fvec3>&>(*attribute_it);
	}
	
	// Allocate model
	auto model = std::make_unique<render::model>();
	
	// Init model bounds
	auto& bounds = model->get_bounds();
	bounds = {math::fvec3::infinity(), -math::fvec3::infinity()};
	
	// Construct model VAO
	std::size_t vertex_stride = 0;
	std::vector<gl::vertex_input_attribute> vertex_attributes;
	gl::vertex_input_attribute position_attribute{};
	if (vertex_positions)
	{
		position_attribute.location = render::vertex_attribute_location::position;
		position_attribute.binding = 0;
		position_attribute.format = gl::format::r32g32b32_sfloat;
		position_attribute.offset = 0;
		vertex_attributes.emplace_back(position_attribute);
		
		vertex_stride += 3 * sizeof(float);
	}
	gl::vertex_input_attribute normal_attribute{};
	if (vertex_normals)
	{
		normal_attribute.location = render::vertex_attribute_location::normal;
		normal_attribute.binding = 0;
		normal_attribute.format = gl::format::r32g32b32_sfloat;
		normal_attribute.offset = static_cast<std::uint32_t>(vertex_stride);
		vertex_attributes.emplace_back(normal_attribute);
		
		vertex_stride += 3 * sizeof(float);
	}
	auto& vao = model->get_vertex_array();
	vao = std::make_unique<gl::vertex_array>(vertex_attributes);
	
	// Interleave vertex data
	std::vector<std::byte> vertex_data(mesh.faces().size() * 3 * vertex_stride);
	if (vertex_positions)
	{
		std::byte* v = vertex_data.data() + position_attribute.offset;
		for (auto face: mesh.faces())
		{
			for (auto loop: face->loops())
			{
				const auto& position = (*vertex_positions)[loop->vertex()->index()];
				std::memcpy(v, position.data(), sizeof(float) * 3);
				v += vertex_stride;
				
				// Extend model bounds
				bounds.extend(position);
			}
		}
	}
	if (vertex_normals)
	{
		std::byte* v = vertex_data.data() + normal_attribute.offset;
		for (auto face: mesh.faces())
		{
			for (auto loop: face->loops())
			{
				const auto& normal = (*vertex_normals)[loop->vertex()->index()];
				std::memcpy(v, normal.data(), sizeof(float) * 3);
				v += vertex_stride;
			}
		}
	}
	
	// Construct model VBO
	auto& vbo = model->get_vertex_buffer();
	vbo = std::make_unique<gl::vertex_buffer>(gl::buffer_usage::static_draw, vertex_data);
	model->set_vertex_offset(0);
	model->set_vertex_stride(vertex_stride);
	
	// Create material group
	model->get_groups().resize(1);
	render::model_group& model_group = model->get_groups().front();
	
	model_group.id = {};
	model_group.material = material;
	model_group.primitive_topology = gl::primitive_topology::triangle_list;
	model_group.first_vertex = 0;
	model_group.vertex_count = static_cast<std::uint32_t>(mesh.faces().size() * 3);
	
	return model;
}

} // namespace geom

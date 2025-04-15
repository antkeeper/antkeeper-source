// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.geom.brep.operations;
import engine.math.functions;
import engine.math.vector;

namespace engine::geom::brep
{
	void generate_face_normals(mesh& mesh)
	{
		const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
		auto& face_normals = static_cast<attribute<math::fvec3>&>(*mesh.faces().attributes().try_emplace<math::fvec3>("normal").first);

		for (face* face : mesh.faces())
		{
			auto loop = face->loops().begin();
			const auto& a = vertex_positions[loop->vertex()->index()];
			const auto& b = vertex_positions[(++loop)->vertex()->index()];
			const auto& c = vertex_positions[(++loop)->vertex()->index()];

			face_normals[face->index()] = math::normalize(math::cross(b - a, c - a));
		}
	}

	void generate_vertex_normals(mesh& mesh)
	{
		// Generate face normals if they don't exist
		if (!mesh.faces().attributes().contains("normal"))
		{
			generate_face_normals(mesh);
		}

		const auto& vertex_positions = mesh.vertices().attributes().at<math::fvec3>("position");
		const auto& face_normals = mesh.faces().attributes().at<math::fvec3>("normal");
		auto& vertex_normals = static_cast<attribute<math::fvec3>&>(*mesh.vertices().attributes().try_emplace<math::fvec3>("normal").first);

		for (vertex* vertex : mesh.vertices())
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
			for (edge* edge : vertex->edges())
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
				for (loop* loop : edge->loops())
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
					const auto edge_angle = math::acos(cos_edge_angle);

					// Weigh face normal by edge angle and add to vertex normal
					vertex_normal += face_normals[loop->face()->index()] * edge_angle;
				}
			}

			// Normalize vertex normal
			const auto sqr_length = math::sqr_length(vertex_normal);
			if (sqr_length)
			{
				vertex_normal /= math::sqrt(sqr_length);
			}
		}
	}

	void generate_loop_barycentric(mesh& mesh)
	{
		auto& loop_barycentric = static_cast<attribute<math::fvec3>&>(*mesh.loops().attributes().try_emplace<math::fvec3>("barycentric").first);

		for (face* face : mesh.faces())
		{
			auto loop = face->loops().begin();
			loop_barycentric[loop->index()] = {1.0f, 0.0f, 0.0f};
			loop_barycentric[(++loop)->index()] = {0.0f, 1.0f, 0.0f};
			loop_barycentric[(++loop)->index()] = {0.0f, 0.0f, 1.0f};
		}
	}
}

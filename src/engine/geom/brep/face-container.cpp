// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/geom/brep/face-container.hpp>
#include <engine/geom/brep/mesh.hpp>
#include <engine/utility/sized-types.hpp>
#include <utility>
#include <vector>

namespace engine::geom::brep
{
	face* face_container::emplace_back(const std::span<vertex*> vertices)
	{
		if (vertices.size() < 3)
		{
			return nullptr;
		}

		// Find or make edges
		std::vector<edge*> edges(vertices.size());
		{
			usize i = vertices.size() - 1;
			for (usize j = 0; j < vertices.size(); ++j)
			{
				edges[i] = m_mesh->edges().find(vertices[i], vertices[j]);
				if (!edges[i])
				{
					edges[i] = m_mesh->edges().emplace_back(vertices[i], vertices[j]);
				}

				i = j;
			}
		}

		// Allocate face
		auto face = element_container<brep::face>::emplace_back();
		face->m_index = size() - 1;

		// Make face loops
		for (usize i = 0; i < vertices.size(); ++i)
		{
			loop* loop = m_mesh->loops().emplace_back();

			loop->m_vertex = vertices[i];
			loop->m_edge = edges[i];
			loop->m_face = face;

			// Append loop to its edge's list of loops
			loop->m_edge->m_loops.push_back(loop);

			// Append loop to its face's list of loops
			face->m_loops.push_back(loop);
		}

		return face;
	};

	void face_container::erase(brep::face* face)
	{
		auto loop = face->loops().front();
		do
		{
			// Remove loop from its edge's list of loops
			loop->m_edge->m_loops.remove(loop);

			auto next_loop = loop->m_face_next;

			// Erase loop
			m_mesh->loops().erase(loop);

			loop = next_loop;
		} while (loop != face->loops().front());

		// Erase face
		element_container<brep::face>::erase(face);
	}

	void face_container::clear() noexcept
	{
		while (!empty())
		{
			erase(back());
		}
	}

	void face_container::reverse(brep::face* face)
	{
		for (auto loop : face->loops())
		{
			// Swap order of loop vertices
			loop->m_vertex = loop->edge()->vertices()[loop->edge()->vertices()[0] == loop->vertex()];

			// Swap pointers to next and previous face loops
			std::swap(loop->m_face_next, loop->m_face_previous);
		}
	}
}

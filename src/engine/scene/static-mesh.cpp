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

#include <engine/scene/static-mesh.hpp>
#include <engine/render/model.hpp>
#include <engine/render/material.hpp>
#include <engine/scene/camera.hpp>

namespace scene {

static_mesh::static_mesh(std::shared_ptr<render::model> model)
{
	set_model(model);
}

void static_mesh::set_model(std::shared_ptr<render::model> model)
{
	m_model = model;
	
	if (m_model)
	{
		m_operations.resize(m_model->get_groups().size());
		for (std::size_t i = 0; i < m_operations.size(); ++i)
		{
			const auto& group = m_model->get_groups()[i];
			
			auto& operation = m_operations[i];
			operation.vertex_array = m_model->get_vertex_array().get();
			operation.drawing_mode = group.drawing_mode;
			operation.start_index = group.start_index;
			operation.index_count = group.index_count;
			operation.material = group.material;
		}
	}
	else
	{
		m_operations.clear();
	}
	
	transformed();
}

void static_mesh::set_material(std::size_t index, std::shared_ptr<render::material> material)
{
	if (material)
	{
		m_operations[index].material = material;
	}
	else
	{
		m_operations[index].material = m_model->get_groups()[index].material;
	}
}

void static_mesh::reset_materials()
{
	for (std::size_t i = 0; i < m_operations.size(); ++i)
	{
		m_operations[i].material = m_model->get_groups()[i].material;
	}
}

void static_mesh::update_bounds()
{
	if (m_model)
	{
		// Get model bounds
		const auto& model_bounds = m_model->get_bounds();
		
		// Naive algorithm: transform each corner of the model AABB
		m_bounds = {math::fvec3::infinity(), -math::fvec3::infinity()};
		for (std::size_t i = 0; i < 8; ++i)
		{
			m_bounds.extend(get_transform() * model_bounds.corner(i));
		}
	}
	else
	{
		m_bounds = {get_translation(), get_translation()};
	}
}

void static_mesh::transformed()
{
	update_bounds();
	
	const math::fmat4 transform_matrix = get_transform().matrix();
	for (auto& operation: m_operations)
	{
		operation.transform = transform_matrix;
	}
}

void static_mesh::render(render::context& ctx) const
{
	const float depth = ctx.camera->get_view_frustum().near().distance(get_translation());
	for (auto& operation: m_operations)
	{
		operation.depth = depth;
		ctx.operations.push_back(&operation);
	}
}

} // namespace scene

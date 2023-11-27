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

#include <engine/scene/skeletal-mesh.hpp>
#include <engine/scene/camera.hpp>

namespace scene {

skeletal_mesh::skeletal_mesh(std::shared_ptr<render::model> model)
{
	set_model(model);
}

void skeletal_mesh::set_model(std::shared_ptr<render::model> model)
{
	m_model = model;
	
	if (m_model)
	{
		m_pose = animation_pose(model->get_skeleton());
		
		m_operations.resize(m_model->get_groups().size());
		for (std::size_t i = 0; i < m_operations.size(); ++i)
		{
			const auto& group = m_model->get_groups()[i];
			auto& operation = m_operations[i];
			
			operation.primitive_topology = group.primitive_topology;
			operation.vertex_array = m_model->get_vertex_array().get();
			operation.vertex_buffer = m_model->get_vertex_buffer().get();
			operation.vertex_offset = m_model->get_vertex_offset();
			operation.vertex_stride = m_model->get_vertex_stride();
			operation.first_vertex = group.first_vertex;
			operation.vertex_count = group.vertex_count;
			operation.first_instance = 0;
			operation.instance_count = 1;
			operation.material = group.material;
			operation.matrix_palette = m_pose.get_matrix_palette();
		}
		
	}
	else
	{
		m_operations.clear();
	}
	
	transformed();
}

void skeletal_mesh::set_material(std::size_t index, std::shared_ptr<render::material> material)
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

void skeletal_mesh::reset_materials()
{
	for (std::size_t i = 0; i < m_operations.size(); ++i)
	{
		m_operations[i].material = m_model->get_groups()[i].material;
	}
}

void skeletal_mesh::update_bounds()
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

void skeletal_mesh::transformed()
{
	update_bounds();
	
	const math::fmat4 transform_matrix = get_transform().matrix();
	for (auto& operation: m_operations)
	{
		operation.transform = transform_matrix;
	}
}

void skeletal_mesh::render(render::context& ctx) const
{
	const float depth = ctx.camera->get_view_frustum().near().distance(get_translation());
	for (auto& operation: m_operations)
	{
		operation.depth = depth;
		operation.layer_mask = get_layer_mask();
		ctx.operations.push_back(&operation);
	}
}

} // namespace scene

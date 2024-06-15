// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

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
		m_pose = skeleton_pose(model->get_skeleton());
		
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
			operation.skinning_matrices = m_pose.get_skinning_matrices();
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
		m_bounds = {math::inf<math::fvec3>, -math::inf<math::fvec3>};
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
	// Update skinning matrices
	m_pose.update_skinning_matrices();

	const float depth = ctx.camera->get_view_frustum().near().distance(get_translation());

	for (auto& operation: m_operations)
	{
		operation.depth = depth;
		operation.layer_mask = get_layer_mask();
		ctx.operations.push_back(&operation);
	}
}

} // namespace scene

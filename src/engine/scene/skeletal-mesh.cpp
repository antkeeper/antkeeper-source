// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

import engine.scene.skeletal_mesh;
import engine.scene.camera;
import engine.render.context;
import engine.utility.sized_types;
import <stdexcept>;

namespace engine::scene
{
	skeletal_mesh::skeletal_mesh(std::shared_ptr<render::model> model)
	{
		set_model(std::move(model));
	}

	void skeletal_mesh::set_model(std::shared_ptr<render::model> model)
	{
		if (model)
		{
			if (!model->skeleton())
			{
				throw std::runtime_error("Failed to set skeletal mesh model: model has no skeleton.");
			}

			m_model = std::move(model);
			m_pose = animation::skeleton_pose(*m_model->skeleton());

			m_operations.resize(m_model->get_groups().size());
			for (usize i = 0; i < m_operations.size(); ++i)
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
				operation.material = m_model->materials().at(group.material_index);
				operation.skinning_matrices = m_pose.get_skinning_matrices();
			}

		}
		else
		{
			m_model = nullptr;
			m_operations.clear();
		}

		transformed();
	}

	void skeletal_mesh::set_material(usize index, std::shared_ptr<render::material> material)
	{
		if (material)
		{
			m_operations.at(index).material = material;
		}
		else
		{
			m_operations.at(index).material = m_model->materials().at(m_model->get_groups().at(index).material_index);
		}
	}

	void skeletal_mesh::reset_materials()
	{
		for (usize i = 0; i < m_operations.size(); ++i)
		{
			m_operations[i].material = m_model->materials().at(m_model->get_groups().at(i).material_index);
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
			for (usize i = 0; i < 8; ++i)
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
		for (auto& operation : m_operations)
		{
			operation.transform = transform_matrix;
		}
	}

	void skeletal_mesh::render(render::context& ctx) const
	{
		// Update skinning matrices
		m_pose.update_skinning_matrices();

		const float depth = ctx.camera->get_view_frustum().near().distance(get_translation());

		for (auto& operation : m_operations)
		{
			operation.depth = depth;
			operation.layer_mask = get_layer_mask();
			ctx.operations.push_back(&operation);
		}
	}
}

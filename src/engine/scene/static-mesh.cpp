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

namespace scene {

static_mesh::static_mesh(std::shared_ptr<render::model> model)
{
	set_model(model);
}

void static_mesh::set_model(std::shared_ptr<render::model> model)
{
	this->model = model;
	
	if (model)
	{
		operations.resize(model->get_groups().size());
		for (std::size_t i = 0; i < operations.size(); ++i)
		{
			const auto& group = model->get_groups()[i];
			
			auto& operation = operations[i];
			operation.vertex_array = model->get_vertex_array().get();
			operation.drawing_mode = group.drawing_mode;
			operation.start_index = group.start_index;
			operation.index_count = group.index_count;
			operation.material = group.material;
		}
		
		pose = model->get_skeleton().bind_pose;
		::concatenate(pose, pose);
	}
	else
	{
		operations.clear();
		pose.clear();
	}
	
	update_bounds();
}

void static_mesh::set_material(std::size_t index, std::shared_ptr<render::material> material)
{
	if (material)
	{
		operations[index].material = material;
	}
	else
	{
		operations[index].material = model->get_groups()[index].material;
	}
}

void static_mesh::reset_materials()
{
	for (std::size_t i = 0; i < operations.size(); ++i)
	{
		operations[i].material = model->get_groups()[i].material;
	}
}

void static_mesh::update_bounds()
{
	if (model)
	{
		local_bounds = aabb_type::transform(model->get_bounds(), get_transform());
		transformed();
	}
	else
	{
		local_bounds = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
		world_bounds = {get_translation(), get_translation()};
	}
}

void static_mesh::transformed()
{
	world_bounds = aabb_type::transform(local_bounds, get_transform());
}

void static_mesh::update_tweens()
{
	object_base::update_tweens();
}

void static_mesh::render(render::context& ctx) const
{
	const float4x4 transform = math::matrix_cast(get_transform_tween().interpolate(ctx.alpha));
	
	for (auto& operation: operations)
	{
		operation.transform = transform;
		operation.depth = ctx.clip_near.signed_distance(float3(operation.transform[3]));
		ctx.operations.push_back(&operation);
	}
}

} // namespace scene

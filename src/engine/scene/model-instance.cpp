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

#include <engine/scene/model-instance.hpp>
#include <engine/render/model.hpp>
#include <engine/render/material.hpp>

namespace scene {

model_instance::model_instance(std::shared_ptr<render::model> model)
{
	set_model(model);
}

void model_instance::set_model(std::shared_ptr<render::model> model)
{
	this->model = model;
	
	if (model)
	{
		materials.resize(model->get_groups().size());
		reset_materials();
		
		pose = model->get_skeleton().bind_pose;
		::concatenate(pose, pose);
	}
	else
	{
		pose.clear();
	}
	
	update_bounds();
}

void model_instance::set_material(std::size_t group_index, std::shared_ptr<render::material> material)
{
	materials[group_index] = material;
}

void model_instance::set_instanced(bool instanced, std::size_t instance_count)
{
	this->instanced = instanced;
	this->instance_count = (instanced) ? instance_count : 0;
}

void model_instance::reset_materials()
{
	std::fill(materials.begin(), materials.end(), nullptr);
}

void model_instance::update_bounds()
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

void model_instance::transformed()
{
	world_bounds = aabb_type::transform(local_bounds, get_transform());
}

void model_instance::update_tweens()
{
	object_base::update_tweens();
	
	// Update material override tweens
	for (auto& material: materials)
	{
		if (material)
		{
			//material->update_tweens();
		}
	}
}

} // namespace scene

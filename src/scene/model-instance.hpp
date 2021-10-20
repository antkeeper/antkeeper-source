/*
 * Copyright (C) 2021  Christopher J. Howard
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

#ifndef ANTKEEPER_SCENE_MODEL_INSTANCE_HPP
#define ANTKEEPER_SCENE_MODEL_INSTANCE_HPP

#include "scene/object.hpp"
#include "geom/aabb.hpp"
#include <vector>

class material;
class model;
class pose;

namespace scene {

class model_instance: public object<model_instance>
{
public:
	typedef geom::aabb<float> aabb_type;
	
	explicit model_instance(model* model);
	model_instance();
	model_instance(const model_instance& other);
	model_instance& operator=(const model_instance& other);

	/**
	 * Sets the model with which this model instance is associated. This will reset the pose and all overwritten materials.
	 */
	void set_model(model* model);

	/**
	 * Sets the
	 */
	void set_pose(pose* pose);

	/**
	 * Overwrites the material of a model group for this model instance.
	 *
	 * @param group_index Index of a model group.
	 * @param material Pointer to the material which should overwrite the model group's material. A value of `nullptr` indicates the material will not be overwritten.
	 */
	void set_material(std::size_t group_index, material* material);
	
	void set_instanced(bool instanced, std::size_t instance_count = 1);

	/**
	 * Resets all overwritten materials.
	 */
	void reset_materials();
	
	virtual const bounding_volume_type& get_local_bounds() const;
	virtual const bounding_volume_type& get_world_bounds() const;

	const model* get_model() const;
	model* get_model();

	const pose* get_pose() const;
	pose* get_pose();

	const std::vector<material*>* get_materials() const;
	
	bool is_instanced() const;
	std::size_t get_instance_count() const;
	
	virtual void update_tweens();
	
	void update_bounds();

private:
	virtual void transformed();
	
	model* model;
	pose* pose;
	std::vector<material*> materials;
	aabb_type local_bounds;
	aabb_type world_bounds;
	bool instanced;
	std::size_t instance_count;
};

inline const typename object_base::bounding_volume_type& model_instance::get_local_bounds() const
{
	return local_bounds;
}

inline const typename object_base::bounding_volume_type& model_instance::get_world_bounds() const
{
	return world_bounds;
}

inline const model* model_instance::get_model() const
{
	return model;
}

inline model* model_instance::get_model()
{
	return model;
}

inline const pose* model_instance::get_pose() const
{
	return pose;
}

inline pose* model_instance::get_pose()
{	
	return pose;
}

inline const std::vector<material*>* model_instance::get_materials() const
{
	return &materials;
}

inline bool model_instance::is_instanced() const
{
	return instanced;
}

inline std::size_t model_instance::get_instance_count() const
{
	return instance_count;
}

} // namespace scene

#endif // ANTKEEPER_SCENE_MODEL_INSTANCE_HPP


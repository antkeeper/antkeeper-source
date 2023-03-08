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

#ifndef ANTKEEPER_SCENE_MODEL_INSTANCE_HPP
#define ANTKEEPER_SCENE_MODEL_INSTANCE_HPP

#include <engine/scene/object.hpp>
#include <engine/animation/pose.hpp>
#include <engine/geom/aabb.hpp>
#include <engine/render/model.hpp>
#include <vector>

namespace scene {

class model_instance: public object<model_instance>
{
public:
	typedef geom::aabb<float> aabb_type;
	
	/**
	 * Constructs a model instance and sets its model.
	 *
	 * @param model Model with which to associate the model instance.
	 */
	explicit model_instance(std::shared_ptr<render::model> model);
	
	/**
	 * Constructs a model instance.
	 */
	model_instance() = default;

	/**
	 * Sets the model with which this model instance is associated. This will reset the pose and all overwritten materials.
	 */
	void set_model(std::shared_ptr<render::model> model);

	/**
	 * Overwrites the material of a model group for this model instance.
	 *
	 * @param group_index Index of a model group.
	 * @param material Pointer to the material which should overwrite the model group's material. A value of `nullptr` indicates the material will not be overwritten.
	 */
	void set_material(std::size_t group_index, std::shared_ptr<render::material> material);
	
	void set_instanced(bool instanced, std::size_t instance_count = 1);
	
	/**
	 * Resets all overwritten materials.
	 */
	void reset_materials();
	
	/**
	 * Returns the local bounds of the model instance.
	 */
	[[nodiscard]] inline virtual const bounding_volume_type& get_local_bounds() const noexcept
	{
		return local_bounds;
	}
	
	/**
	 * Returns the world bounds of the model instance.
	 */
	[[nodiscard]] inline virtual const bounding_volume_type& get_world_bounds() const noexcept
	{
		return world_bounds;
	}
	
	/**
	 * Returns the model of the model instance.
	 */
	[[nodiscard]] inline const std::shared_ptr<render::model>& get_model() const noexcept
	{
		return model;
	}
	
	/**
	 * Returns the skeletal animation pose of this model instance.
	 */
	/// @{
	[[nodiscard]] inline const pose& get_pose() const noexcept
	{
		return pose;
	}
	[[nodiscard]] inline pose& get_pose() noexcept
	{
		return pose;
	}
	/// @}
	
	/**
	 * Returns the materials of this model instance.
	 */
	[[nodiscard]] inline const std::vector<std::shared_ptr<render::material>>& get_materials() const noexcept
	{
		return materials;
	}
	
	/**
	 * Returns `true` if the model instance is instanced, `false` otherwise.
	 */
	[[nodiscard]] inline bool is_instanced() const noexcept
	{
		return instanced;
	}
	
	/**
	 * Returns the number of instances, if the model is instanced.
	 */
	[[nodiscard]] inline std::size_t get_instance_count() const noexcept
	{
		return instance_count;
	}
	
	virtual void update_tweens();
	
	void update_bounds();

private:
	virtual void transformed();
	
	std::shared_ptr<render::model> model;
	::pose pose;
	std::vector<std::shared_ptr<render::material>> materials;
	aabb_type local_bounds{{0, 0, 0}, {0, 0, 0}};
	aabb_type world_bounds{{0, 0, 0}, {0, 0, 0}};
	bool instanced{false};
	std::size_t instance_count{0};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_MODEL_INSTANCE_HPP

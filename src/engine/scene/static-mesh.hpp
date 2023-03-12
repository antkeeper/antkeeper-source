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

#ifndef ANTKEEPER_SCENE_STATIC_MESH_HPP
#define ANTKEEPER_SCENE_STATIC_MESH_HPP

#include <engine/scene/object.hpp>
#include <engine/animation/pose.hpp>
#include <engine/geom/aabb.hpp>
#include <engine/render/model.hpp>
#include <engine/render/operation.hpp>
#include <vector>

namespace scene {

/**
 *
 */
class static_mesh: public object<static_mesh>
{
public:
	typedef geom::aabb<float> aabb_type;
	
	/**
	 * Constructs a static mesh from a model.
	 *
	 * @param model Model from which the static mesh will be constructed.
	 */
	explicit static_mesh(std::shared_ptr<render::model> model);
	
	/**
	 * Constructs a model instance.
	 */
	static_mesh() = default;
	
	/**
	 * Sets the model with which this model instance is associated. This will reset the pose and all overwritten materials.
	 */
	void set_model(std::shared_ptr<render::model> model);
	
	/**
	 * Overwrites the material of a model group for this model instance.
	 *
	 * @param index Index of a model group.
	 * @param material Pointer to the material which should overwrite the model group's material. A value of `nullptr` indicates the material will not be overwritten.
	 */
	void set_material(std::size_t index, std::shared_ptr<render::material> material);
	
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
	
	void render(render::context& ctx) const override;
	
	void update_tweens() override;
	
	void update_bounds();

private:
	void transformed() override;
	
	std::shared_ptr<render::model> model;
	mutable std::vector<render::operation> operations;
	::pose pose;
	
	aabb_type local_bounds{{0, 0, 0}, {0, 0, 0}};
	aabb_type world_bounds{{0, 0, 0}, {0, 0, 0}};
};

} // namespace scene

#endif // ANTKEEPER_SCENE_STATIC_MESH_HPP

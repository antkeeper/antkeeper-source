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

#ifndef ANTKEEPER_SCENE_SKELETAL_MESH_HPP
#define ANTKEEPER_SCENE_SKELETAL_MESH_HPP

#include <engine/scene/object.hpp>
#include <engine/render/model.hpp>
#include <engine/render/operation.hpp>
#include <engine/animation/animation-pose.hpp>
#include <vector>

namespace scene {

/**
 *
 */
class skeletal_mesh: public object<skeletal_mesh>
{
public:
	/**
	 * Constructs a skeletal mesh from a model.
	 *
	 * @param model Model from which the skeletal mesh will be constructed.
	 */
	explicit skeletal_mesh(std::shared_ptr<render::model> model);
	
	/**
	 * Constructs a model instance.
	 */
	skeletal_mesh() = default;
	
	/**
	 * Sets the model with which this model instance is associated.
	 *
	 * @warning This will reset all overwritten materials.
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
	
	[[nodiscard]] inline const aabb_type& get_bounds() const noexcept override
	{
		return m_bounds;
	}
	
	/**
	 * Returns the model of the model instance.
	 */
	[[nodiscard]] inline const std::shared_ptr<render::model>& get_model() const noexcept
	{
		return m_model;
	}
	
	void render(render::context& ctx) const override;
	
	/// Returns the animation pose of the skeletal mesh.
	/// @{
	[[nodiscard]] inline const animation_pose& get_pose() const noexcept
	{
		return m_pose;
	}
	
	[[nodiscard]] inline animation_pose& get_pose() noexcept
	{
		return m_pose;
	}
	/// @}
	
private:
	void update_bounds();
	void transformed() override;
	
	std::shared_ptr<render::model> m_model;
	mutable std::vector<render::operation> m_operations;
	aabb_type m_bounds{{0, 0, 0}, {0, 0, 0}};
	animation_pose m_pose;
};

} // namespace scene

#endif // ANTKEEPER_SCENE_SKELETAL_MESH_HPP

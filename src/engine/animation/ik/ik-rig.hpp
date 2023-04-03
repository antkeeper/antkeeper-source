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

#ifndef ANTKEEPER_ANIMATION_IK_RIG_HPP
#define ANTKEEPER_ANIMATION_IK_RIG_HPP

#include <engine/scene/skeletal-mesh.hpp>
#include <engine/animation/ik/ik-constraint.hpp>
#include <engine/animation/ik/ik-solver.hpp>
#include <memory>
#include <vector>

/**
 * 
 */
class ik_rig
{
public:
	/**
	 * Constructs an IK rig.
	 *
	 * @param skeletal_mesh Skeletal mesh with which to associate the IK rig.
	 */
	explicit ik_rig(scene::skeletal_mesh& skeletal_mesh);
	
	/// Returns the skeleton with which the IK rig is associated.
	/// @{
	[[nodiscard]] inline const scene::skeletal_mesh& get_skeletal_mesh() const noexcept
	{
		return *m_skeletal_mesh;
	}
	[[nodiscard]] inline scene::skeletal_mesh& get_skeletal_mesh() noexcept
	{
		return *m_skeletal_mesh;
	}
	/// @}
	
	/// @name Constraints
	/// @{
	
	/**
	 * Sets the IK constraint of a bone.
	 *
	 * @param index Index of a bone.
	 * @param constraint IK constraint of the bone.
	 */
	void set_constraint(bone_index_type index, std::shared_ptr<ik_constraint> constraint);
	
	/// Removes all constraints from the IK rig.
	void clear_constraints();
	
	/**
	 * Returns the IK constraint of a bone.
	 *
	 * @param index Index of a bone.
	 *
	 * @return Pointer to the IK constraint of the bone, or `nullptr` if the bone is unconstrained.
	 */
	/// @{
	[[nodiscard]] inline const ik_constraint* get_constraint(bone_index_type index) const
	{
		return m_constraints[index].get();
	}
	[[nodiscard]] inline ik_constraint* get_constraint(bone_index_type index)
	{
		return m_constraints[index].get();
	}
	/// @}
	
	/// @}
	
	/// @name Solvers
	/// @{
	
	/**
	 * Solves each solver in the IK rig.
	 */
	void solve();
	
	/**
	 * Adds a solver to the IK rig.
	 *
	 * @param solver IK solver to add.
	 */
	void add_solver(std::shared_ptr<ik_solver> solver);
	
	/**
	 * Removes all solvers from the IK rig.
	 */
	void remove_solvers();
	
	/// @}
	
private:
	scene::skeletal_mesh* m_skeletal_mesh{nullptr};
	std::vector<std::shared_ptr<ik_constraint>> m_constraints;
	std::vector<std::shared_ptr<ik_solver>> m_solvers;
};

#endif // ANTKEEPER_ANIMATION_IK_RIG_HPP

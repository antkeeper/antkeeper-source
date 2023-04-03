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

#include <engine/animation/ik/solvers/ccd-ik-solver.hpp>
#include <engine/animation/ik/ik-rig.hpp>
#include <engine/debug/log.hpp>
#include <stdexcept>

ccd_ik_solver::ccd_ik_solver(ik_rig& ik_rig, bone_index_type root_bone_index, bone_index_type effector_bone_index):
	m_ik_rig{&ik_rig}
{
	// Get reference to skeleton
	const auto& skeleton = *m_ik_rig->get_skeletal_mesh().get_pose().get_skeleton();
	
	// Validate and count number of bones in bone chain
	std::size_t bone_count = 1;
	for (bone_index_type bone_index = effector_bone_index; bone_index != root_bone_index; ++bone_count)
	{
		const auto parent_bone = skeleton.get_bone_parent(bone_index);
		if (parent_bone == bone_index)
		{
			throw std::invalid_argument("Invalid bone chain");
		}
		
		bone_index = parent_bone;
	}
	
	// Allocate and store bone indices
	m_bone_indices.resize(bone_count);
	m_bone_indices.front() = effector_bone_index;
	for (std::size_t i = 1; i < bone_count; ++i)
	{
		m_bone_indices[i] = skeleton.get_bone_parent(m_bone_indices[i - 1]);
	}
}

void ccd_ik_solver::solve()
{
	// Get reference to skeletal mesh and its pose from the parent IK rig
	auto& skeletal_mesh = m_ik_rig->get_skeletal_mesh();
	auto& pose = skeletal_mesh.get_pose();
	
	// Get pose-space transform of end effector bone
	const auto& ps_effector_bone_transform = pose.get_absolute_transform(m_bone_indices.front());
	
	// Transform goal position into pose-space
	const auto ps_goal_position = m_goal_position * skeletal_mesh.get_transform();
	
	for (std::size_t i = 0; i < m_max_iterations; ++i)
	{
		for (std::size_t j = 0; j < m_bone_indices.size(); ++j)
		{
			// Transform end effector position into pose-space
			auto ps_effector_position = ps_effector_bone_transform * m_effector_position;
			
			// Check if end effector is within tolerable distance to goal position
			const auto sqr_distance = math::sqr_distance(ps_effector_position, ps_goal_position);
			if (sqr_distance <= m_sqr_distance_tolerance)
			{
				return;
			}
			
			// Get index of current bone
			bone_index_type bone_index = m_bone_indices[j];
			
			// Get pose-space and bone-space transforms of current bone
			const auto& ps_bone_transform = pose.get_absolute_transform(bone_index);
			const auto& bs_bone_transform = pose.get_relative_transform(bone_index);
			 
			// Find pose-space direction vector from current bone to end effector
			const auto ps_effector_direction = math::normalize(ps_effector_position - ps_bone_transform.translation);
			
			// Find pose-space direction vector from current bone to IK goal
			const auto ps_goal_direction = math::normalize(ps_goal_position - ps_bone_transform.translation);
			
			// Calculate rotation of current bone that brings effector closer to goal
			auto bone_rotation = math::normalize(math::rotation(ps_effector_direction, ps_goal_direction, 1e-5f) * bs_bone_transform.rotation);
			
			// Apply current bone constraints to rotation
			if (auto* constraint = m_ik_rig->get_constraint(bone_index))
			{
				constraint->solve(bone_rotation);
			}
			
			// Rotate current bone
			pose.set_relative_rotation(bone_index, bone_rotation);
			//pose.update(bone_index, j + 1);
			pose.update();
		}
	}
}

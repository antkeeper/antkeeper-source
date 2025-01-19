// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/ik/solvers/ccd-ik-solver.hpp>
#include <engine/animation/ik/ik-rig.hpp>
#include <engine/debug/log.hpp>
#include <stdexcept>

ccd_ik_solver::ccd_ik_solver(ik_rig& ik_rig, std::size_t root_bone_index, std::size_t effector_bone_index):
	m_ik_rig{&ik_rig}
{
	// Get reference to skeleton
	const auto& skeleton = *m_ik_rig->get_skeletal_mesh().get_pose().get_skeleton();
	
	// Validate and count number of bones in bone chain
	std::size_t bone_count = 1;
	for (auto bone_index = effector_bone_index; bone_index != root_bone_index; ++bone_count)
	{
		const auto parent_bone = skeleton.bones()[bone_index].parent();
		if (!parent_bone)
		{
			throw std::invalid_argument("Invalid bone chain");
		}
		
		bone_index = parent_bone->index();
	}
	
	// Allocate and store bone indices
	m_bone_indices.resize(bone_count);
	m_bone_indices.front() = effector_bone_index;
	for (std::size_t i = 1; i < bone_count; ++i)
	{
		m_bone_indices[i] = skeleton.bones()[m_bone_indices[i - 1]].parent()->index();
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
	const auto ps_goal_center = m_goal_center * skeletal_mesh.get_transform();
	
	for (std::size_t i = 0; i < m_max_iterations; ++i)
	{
		for (std::size_t j = 0; j < m_bone_indices.size(); ++j)
		{
			// Transform end effector position into pose-space
			const auto ps_effector_position = ps_effector_bone_transform * m_effector_position;
			
			// Check if end effector is within goal radius
			const auto sqr_distance = math::sqr_distance(ps_effector_position, ps_goal_center);
			if (sqr_distance <= m_sqr_goal_radius)
			{
				return;
			}
			
			// Get index of current bone
			const auto bone_index = m_bone_indices[j];
			
			// Get pose-space and bone-space transforms of current bone
			const auto& ps_bone_transform = pose.get_absolute_transform(bone_index);
			const auto& bs_bone_transform = pose.get_relative_transform(bone_index);
			 
			// Find pose-space direction vector from current bone to effector
			const auto ps_effector_direction = math::normalize(ps_effector_position - ps_bone_transform.translation);
			
			// Find pose-space direction vector from current bone to center of goal
			const auto ps_goal_direction = math::normalize(ps_goal_center - ps_bone_transform.translation);
			
			// Find rotation for current bone that brings effector closer to goal
			auto bone_rotation = math::normalize(math::rotation(ps_effector_direction, ps_goal_direction, 1e-5f) * bs_bone_transform.rotation);
			
			// Apply current bone constraints to rotation
			if (auto* constraint = m_ik_rig->get_constraint(bone_index))
			{
				constraint->solve(bone_rotation);
			}
			
			// Rotate current bone
			pose.set_relative_rotation(bone_index, bone_rotation);
		}
	}
}

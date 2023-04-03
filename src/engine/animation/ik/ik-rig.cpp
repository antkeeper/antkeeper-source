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

#include <engine/animation/ik/ik-rig.hpp>

ik_rig::ik_rig(scene::skeletal_mesh& skeletal_mesh):
	m_skeletal_mesh(&skeletal_mesh),
	m_constraints(skeletal_mesh.get_pose().get_skeleton()->get_bone_count())
{}

void ik_rig::set_constraint(bone_index_type index, std::shared_ptr<ik_constraint> constraint)
{
	m_constraints[index] = std::move(constraint);
}

void ik_rig::clear_constraints()
{
	for (auto& constraint: m_constraints)
	{
		constraint.reset();
	}
}

void ik_rig::solve()
{
	for (const auto& solver: m_solvers)
	{
		solver->solve();
	}
}

void ik_rig::add_solver(std::shared_ptr<ik_solver> solver)
{
	m_solvers.emplace_back(std::move(solver));
}

void ik_rig::remove_solvers()
{
	m_solvers.clear();
}

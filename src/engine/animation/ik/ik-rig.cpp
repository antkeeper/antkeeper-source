// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <engine/animation/ik/ik-rig.hpp>

ik_rig::ik_rig(scene::skeletal_mesh& skeletal_mesh):
	m_skeletal_mesh(&skeletal_mesh),
	m_constraints(skeletal_mesh.get_pose().get_skeleton()->bones().size())
{}

void ik_rig::set_constraint(std::size_t index, std::shared_ptr<ik_constraint> constraint)
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

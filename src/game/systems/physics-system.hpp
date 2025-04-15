// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PHYSICS_SYSTEM_HPP
#define ANTKEEPER_GAME_PHYSICS_SYSTEM_HPP

#include "game/systems/fixed-update-system.hpp"
import engine.physics.kinematics.rigid_body;
import engine.physics.kinematics.collision;
import engine.entity.id;
import engine.math.vector;
import <array>;
import <functional>;

using namespace engine;

class physics_system:
	public fixed_update_system
{
public:
	physics_system();
	~physics_system() override = default;
	void fixed_update(entity::registry& registry, float t, float dt) override;
	
private:
	using collision_manifold_type = physics::collision_manifold<4>;
	
	void integrate(entity::registry& registry, float dt);
	
	void solve_constraints(entity::registry& registry, float dt);
	
	void detect_collisions_broad(entity::registry& registry);
	void detect_collisions_narrow();
	void resolve_collisions();
	void correct_positions();
	
	void narrow_phase_plane_plane(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_plane_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_plane_box(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_plane_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b);
	
	void narrow_phase_sphere_plane(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_sphere_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_sphere_box(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_sphere_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b);
	
	void narrow_phase_box_plane(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_box_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_box_box(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_box_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b);
	
	void narrow_phase_capsule_plane(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_capsule_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_capsule_box(physics::rigid_body& body_a, physics::rigid_body& body_b);
	void narrow_phase_capsule_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b);
	
	std::array<std::array<std::function<void(physics::rigid_body&, physics::rigid_body&)>, 4>, 4> m_narrow_phase_table;
	std::vector<std::pair<physics::rigid_body*, physics::rigid_body*>> m_broad_phase_pairs;
	std::vector<collision_manifold_type> m_narrow_phase_manifolds;
};

#endif // ANTKEEPER_GAME_PHYSICS_SYSTEM_HPP

// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_GAME_PHYSICS_SYSTEM_HPP
#define ANTKEEPER_GAME_PHYSICS_SYSTEM_HPP

#include "game/systems/updatable-system.hpp"
#include <entt/entt.hpp>
#include <engine/math/vector.hpp>
#include <engine/entity/id.hpp>
#include <engine/physics/kinematics/rigid-body.hpp>
#include <engine/physics/kinematics/collision-manifold.hpp>
#include <engine/geom/primitives/ray.hpp>
#include <array>
#include <functional>
#include <optional>
#include <tuple>

class physics_system:
	public updatable_system
{
public:
	explicit physics_system(entity::registry& registry);
	~physics_system() override = default;
	
	void update(float t, float dt) override;
	
	void interpolate(float alpha);
	
	/// Sets the gravity vector.
	/// @param gravity Gravity vector.
	inline void set_gravity(const math::fvec3& gravity) noexcept
	{
		m_gravity = gravity;
	}
	
	/// Traces a ray to the nearest point of intersection.
	/// @param ray World-spce ray.
	/// @param ignore_eid Entity ID with which to ignore intersection.
	/// @param layer_mask Mask of collision layers with which the ray can intersect.
	/// @return Tuple containing the ID of the nearest intersecting entity, distance along the ray to the point of intersection, index of the hit face, and surface normal at the point of intersection; or std::nullopt if no intersection occurred.
	[[nodiscard]] std::optional<std::tuple<entity::id, float, std::uint32_t, math::fvec3>> trace(const geom::ray<float, 3>& ray, entity::id ignore_eid = entt::null, std::uint32_t layer_mask = ~std::uint32_t{0}) const;
	
private:
	using collision_manifold_type = physics::collision_manifold<4>;
	
	void integrate(float dt);
	
	void solve_constraints(float dt);
	
	void detect_collisions_broad();
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
	
	math::fvec3 m_gravity{0.0f, -9.80665f, 0.0f};
	
	std::vector<std::pair<physics::rigid_body*, physics::rigid_body*>> m_broad_phase_pairs;
	std::vector<collision_manifold_type> m_narrow_phase_manifolds;
};

#endif // ANTKEEPER_GAME_PHYSICS_SYSTEM_HPP

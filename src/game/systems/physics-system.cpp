// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#include <execution>
#include <entt/entt.hpp>
#include "game/systems/physics-system.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/rigid-body-constraint-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/gravity-component.hpp"
#include <engine/physics/kinematics/colliders/plane-collider.hpp>
#include <engine/physics/kinematics/colliders/sphere-collider.hpp>
#include <engine/physics/kinematics/colliders/box-collider.hpp>
#include <engine/physics/kinematics/colliders/capsule-collider.hpp>
#include <engine/physics/kinematics/colliders/mesh-collider.hpp>
#include <engine/geom/closest-point.hpp>
#include <engine/debug/log.hpp>
#include <engine/entity/id.hpp>
#include <engine/utility/sized-types.hpp>
#include <engine/math/functions.hpp>
#include <algorithm>
#include <optional>

using namespace engine;
using namespace engine::math;

physics_system::physics_system()
{
	constexpr auto plane_i = std::to_underlying(physics::collider_type::plane);
	constexpr auto sphere_i = std::to_underlying(physics::collider_type::sphere);
	constexpr auto box_i = std::to_underlying(physics::collider_type::box);
	constexpr auto capsule_i = std::to_underlying(physics::collider_type::capsule);
	
	m_narrow_phase_table[plane_i][plane_i] = std::bind_front(&physics_system::narrow_phase_plane_plane, this);
	m_narrow_phase_table[plane_i][sphere_i] = std::bind_front(&physics_system::narrow_phase_plane_sphere, this);
	m_narrow_phase_table[plane_i][box_i] = std::bind_front(&physics_system::narrow_phase_plane_box, this);
	m_narrow_phase_table[plane_i][capsule_i] = std::bind_front(&physics_system::narrow_phase_plane_capsule, this);
	
	m_narrow_phase_table[sphere_i][plane_i] = std::bind_front(&physics_system::narrow_phase_sphere_plane, this);
	m_narrow_phase_table[sphere_i][sphere_i] = std::bind_front(&physics_system::narrow_phase_sphere_sphere, this);
	m_narrow_phase_table[sphere_i][box_i] = std::bind_front(&physics_system::narrow_phase_sphere_box, this);
	m_narrow_phase_table[sphere_i][capsule_i] = std::bind_front(&physics_system::narrow_phase_sphere_capsule, this);
	
	m_narrow_phase_table[box_i][plane_i] = std::bind_front(&physics_system::narrow_phase_box_plane, this);
	m_narrow_phase_table[box_i][sphere_i] = std::bind_front(&physics_system::narrow_phase_box_sphere, this);
	m_narrow_phase_table[box_i][box_i] = std::bind_front(&physics_system::narrow_phase_box_box, this);
	m_narrow_phase_table[box_i][capsule_i] = std::bind_front(&physics_system::narrow_phase_box_capsule, this);
	
	m_narrow_phase_table[capsule_i][plane_i] = std::bind_front(&physics_system::narrow_phase_capsule_plane, this);
	m_narrow_phase_table[capsule_i][sphere_i] = std::bind_front(&physics_system::narrow_phase_capsule_sphere, this);
	m_narrow_phase_table[capsule_i][box_i] = std::bind_front(&physics_system::narrow_phase_capsule_box, this);
	m_narrow_phase_table[capsule_i][capsule_i] = std::bind_front(&physics_system::narrow_phase_capsule_capsule, this);
}

void physics_system::fixed_update(entity::registry& registry, float, float dt)
{
	detect_collisions_broad(registry);
	detect_collisions_narrow();
	solve_constraints(registry, dt);
	resolve_collisions();
	integrate(registry, dt);
	correct_positions();
	
	// Update transform component transforms
	auto transform_view = registry.view<rigid_body_component, transform_component>();
	for (const auto entity_id: transform_view)
	{
		const auto& body = *(transform_view.get<rigid_body_component>(entity_id).body);
		
		// Update transform
		registry.patch<::transform_component>
		(
			entity_id,
			[&, dt](auto& transform)
			{
				// Integrate position
				transform.local = body.get_transform();
			}
		);
	}
}

void physics_system::integrate(entity::registry& registry, float dt)
{
	std::optional<fvec3> gravity;
	if (auto view = registry.view<gravity_component>(); !view.empty())
	{
		gravity = view.get<gravity_component>(view.front()).force;
	}

	auto view = registry.view<rigid_body_component>();
	std::for_each
	(
		std::execution::par_unseq,
		view.begin(),
		view.end(),
		[&](auto entity_id)
		{
			auto& body = *(view.get<rigid_body_component>(entity_id).body);
			
			// Apply gravity
			if (gravity)
			{
				body.apply_central_force(*gravity * body.get_mass());
			}
			
			body.integrate(dt);
		}
	);
}

void physics_system::solve_constraints(entity::registry& registry, float dt)
{
	registry.view<rigid_body_constraint_component>().each
	(
		[dt](auto& component)
		{
			component.constraint->solve(dt);
		}
	);
}

void physics_system::detect_collisions_broad(entity::registry& registry)
{
	m_broad_phase_pairs.clear();
	
	auto view = registry.view<rigid_body_component>();
	for (auto i = view.begin(); i != view.end(); ++i)
	{
		auto& body_a = *view.get<rigid_body_component>(*i).body;
		
		const auto& collider_a = body_a.get_collider();
		if (!collider_a)
		{
			continue;
		}
		
		for (auto j = i + 1; j != view.end(); ++j)
		{
			auto& body_b = *view.get<rigid_body_component>(*j).body;
			
			const auto& collider_b = body_b.get_collider();
			if (!collider_b)
			{
				continue;
			}
			
			// Ignore pairs without a mutual layer
			if (!(collider_a->get_layer_mask() & collider_b->get_layer_mask()))
			{
				continue;
			}
			
			// Ignore static pairs
			if (body_a.is_static() && body_b.is_static())
			{
				continue;
			}
			
			m_broad_phase_pairs.emplace_back(&body_a, &body_b);
		}
	}
}

void physics_system::detect_collisions_narrow()
{
	m_narrow_phase_manifolds.clear();
	
	for (const auto& pair: m_broad_phase_pairs)
	{
		auto& body_a = *pair.first;
		auto& body_b = *pair.second;
		
		m_narrow_phase_table[std::to_underlying(body_a.get_collider()->type())][std::to_underlying(body_b.get_collider()->type())](body_a, body_b);
	}
}

void physics_system::resolve_collisions()
{
	for (const auto& manifold: m_narrow_phase_manifolds)
	{
		auto& body_a = *manifold.body_a;
		auto& body_b = *manifold.body_b;
		
		const auto& material_a = *body_a.get_collider()->get_material();
		const auto& material_b = *body_b.get_collider()->get_material();
		
		// Calculate coefficient of restitution
		const auto restitution_combine_mode = max(material_a.get_restitution_combine_mode(), material_b.get_restitution_combine_mode());
		float restitution_coef = physics::combine_restitution(material_a.get_restitution(), material_b.get_restitution(), restitution_combine_mode);
		
		// Calculate coefficients of friction
		const auto friction_combine_mode = max(material_a.get_friction_combine_mode(), material_b.get_friction_combine_mode());
		float static_friction_coef = physics::combine_friction(material_a.get_static_friction(), material_b.get_static_friction(), friction_combine_mode);
		float dynamic_friction_coef = physics::combine_friction(material_a.get_dynamic_friction(), material_b.get_dynamic_friction(), friction_combine_mode);
		
		const float sum_inverse_mass = body_a.get_inverse_mass() + body_b.get_inverse_mass();
		const float impulse_scale = 1.0f / static_cast<float>(manifold.contact_count);
		
		for (u8 i = 0; i < manifold.contact_count; ++i)
		{
			const physics::collision_contact& contact = manifold.contacts[i];
			
			const fvec3 radius_a = contact.point - body_a.get_position();
			const fvec3 radius_b = contact.point - body_b.get_position();
			
			fvec3 relative_velocity = body_b.get_point_velocity(radius_b) - body_a.get_point_velocity(radius_a);
			
			const float contact_velocity = dot(relative_velocity, contact.normal);
			if (contact_velocity > 0.0f)
			{
				continue;
			}
			
			const float reaction_impulse_num = -(1.0f + restitution_coef) * contact_velocity;
			const fvec3 ra_cross_n = cross(radius_a, contact.normal);
			const fvec3 rb_cross_n = cross(radius_b, contact.normal);
			const float reaction_impulse_den = sum_inverse_mass +
				dot
				(
					cross(body_a.get_inverse_inertia() * ra_cross_n, radius_a) +
					cross(body_b.get_inverse_inertia() * rb_cross_n, radius_b),
					contact.normal
				);
			const float reaction_impulse_mag = (reaction_impulse_num / reaction_impulse_den) * impulse_scale;
			const fvec3 reaction_impulse = contact.normal * reaction_impulse_mag;
			
			// Apply reaction impulses
			body_a.apply_impulse(-reaction_impulse, radius_a);
			body_b.apply_impulse(reaction_impulse, radius_b);
			
			//relative_velocity = body_b.get_point_velocity(radius_b) - body_a.get_point_velocity(radius_a);
			
			fvec3 contact_tangent = relative_velocity - contact.normal * contact_velocity;
			const float sqr_tangent_length = sqr_length(contact_tangent);
			if (sqr_tangent_length > 0.0f)
			{
				contact_tangent /= sqrt(sqr_tangent_length);
			}
					
			const float friction_impulse_num = dot(relative_velocity, -contact_tangent);
			const fvec3 ra_cross_t = cross(radius_a, contact_tangent);
			const fvec3 rb_cross_t = cross(radius_b, contact_tangent);
			const float friction_impulse_den = sum_inverse_mass +
				dot
				(
					cross(body_a.get_inverse_inertia() * ra_cross_t, radius_a) +
					cross(body_b.get_inverse_inertia() * rb_cross_t, radius_b),
					contact_tangent
				);
			float friction_impulse_mag = (friction_impulse_num / friction_impulse_den) * impulse_scale;
			
			if (abs(friction_impulse_mag) >= reaction_impulse_mag * static_friction_coef)
			{
				friction_impulse_mag = -reaction_impulse_mag * dynamic_friction_coef;
			}
			
			const fvec3 friction_impulse = contact_tangent * friction_impulse_mag;
			
			body_a.apply_impulse(-friction_impulse, radius_a);
			body_b.apply_impulse(friction_impulse, radius_b);
		}
	}
}

void physics_system::correct_positions()
{
	const float depth_threshold = 0.01f;
	const float correction_factor = 0.4f;
	
	for (const auto& manifold: m_narrow_phase_manifolds)
	{
		auto& body_a = *manifold.body_a;
		auto& body_b = *manifold.body_b;
		const float sum_inverse_mass = body_a.get_inverse_mass() + body_b.get_inverse_mass();
		
		for (u8 i = 0; i < manifold.contact_count; ++i)
		{
			const physics::collision_contact& contact = manifold.contacts[i];
			
			fvec3 correction = contact.normal * (max(0.0f, contact.depth - depth_threshold) / sum_inverse_mass) * correction_factor;
			
			body_a.set_position(body_a.get_position() - correction * body_a.get_inverse_mass());
			body_b.set_position(body_b.get_position() + correction * body_b.get_inverse_mass());
		}
	}
}

void physics_system::narrow_phase_plane_plane(physics::rigid_body&, physics::rigid_body&)
{
	return;
}

void physics_system::narrow_phase_plane_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& plane_a = static_cast<const physics::plane_collider&>(*body_a.get_collider());
	const auto& sphere_b = static_cast<const physics::sphere_collider&>(*body_b.get_collider());
	
	// Transform plane into world-space
	const fvec3 plane_normal = body_a.get_orientation() * plane_a.get_normal();
	const float plane_constant = plane_a.get_constant() - dot(plane_normal, body_a.get_position());
	
	const float signed_distance = dot(plane_normal, body_b.get_position()) + plane_constant;
	if (signed_distance > sphere_b.get_radius())
	{
		return;
	}
	
	// Init collision manifold
	collision_manifold_type manifold;
	manifold.body_a = &body_a;
	manifold.body_b = &body_b;
	manifold.contact_count = 1;
	
	// Generate collision contact
	auto& contact = manifold.contacts[0];
	contact.point = body_b.get_position() - plane_normal * sphere_b.get_radius();
	contact.normal = plane_normal;
	contact.depth = abs(signed_distance - sphere_b.get_radius());
	
	m_narrow_phase_manifolds.emplace_back(std::move(manifold));
}

void physics_system::narrow_phase_plane_box(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& plane_a = static_cast<const physics::plane_collider&>(*body_a.get_collider());
	const auto& box_b = static_cast<const physics::box_collider&>(*body_b.get_collider());
	
	// Transform plane into world-space
	const fvec3 plane_normal = body_a.get_orientation() * plane_a.get_normal();
	const float plane_constant = plane_a.get_constant() - dot(plane_normal, body_a.get_position());
	
	const auto& box_min = box_b.get_min();
	const auto& box_max = box_b.get_max();
	const fvec3 corners[8] =
	{
		{box_min.x(), box_min.y(), box_min.z()},
		{box_min.x(), box_min.y(), box_max.z()},
		{box_min.x(), box_max.y(), box_min.z()},
		{box_min.x(), box_max.y(), box_max.z()},
		{box_max.x(), box_min.y(), box_min.z()},
		{box_max.x(), box_min.y(), box_max.z()},
		{box_max.x(), box_max.y(), box_min.z()},
		{box_max.x(), box_max.y(), box_max.z()}
	};
	
	// Init collision manifold
	collision_manifold_type manifold;
	manifold.contact_count = 0;
	
	// Brute force
	for (usize i = 0; i < 8; ++i)
	{
		// Transform corner into world-space
		const fvec3 point = body_b.get_transform() * corners[i];
		
		const float signed_distance = dot(plane_normal, point) + plane_constant;
		
		if (signed_distance <= 0.0f)
		{
			auto& contact = manifold.contacts[manifold.contact_count];
			contact.point = point;
			contact.normal = plane_normal;
			contact.depth = abs(signed_distance);
			
			++manifold.contact_count;
			
			if (manifold.contact_count >= 4)
			{
				break;
			}
		}
	}
	
	if (manifold.contact_count)
	{
		manifold.body_a = &body_a;
		manifold.body_b = &body_b;
		m_narrow_phase_manifolds.emplace_back(std::move(manifold));
	}
}

void physics_system::narrow_phase_plane_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& plane_a = static_cast<const physics::plane_collider&>(*body_a.get_collider());
	const auto& capsule_b = static_cast<const physics::capsule_collider&>(*body_b.get_collider());
	
	// Transform plane into world-space
	geom::plane<float> plane;
	plane.normal = body_a.get_orientation() * plane_a.get_normal();
	plane.constant = plane_a.get_constant() - dot(plane.normal, body_a.get_position());
	
	// Transform capsule into world-space
	const geom::capsule<float> capsule
	{
		{
			body_b.get_transform() * capsule_b.get_segment().a,
			body_b.get_transform() * capsule_b.get_segment().b
		},
		capsule_b.get_radius()
	};
	
	// Calculate signed distances to capsule segment endpoints
	const float distance_a = plane.distance(capsule.segment.a);
	const float distance_b = plane.distance(capsule.segment.b);
	
	collision_manifold_type manifold;
	manifold.contact_count = 0;
	
	if (distance_a <= capsule.radius)
	{
		auto& contact = manifold.contacts[manifold.contact_count];
		
		contact.point = capsule.segment.a - plane.normal * capsule.radius;
		contact.normal = plane.normal;
		contact.depth = abs(distance_a - capsule.radius);
		
		++manifold.contact_count;
	}
	
	if (distance_b <= capsule.radius)
	{
		auto& contact = manifold.contacts[manifold.contact_count];
		
		contact.point = capsule.segment.b - plane.normal * capsule.radius;
		contact.normal = plane.normal;
		contact.depth = abs(distance_b - capsule.radius);
		
		++manifold.contact_count;
	}
	
	if (manifold.contact_count)
	{
		manifold.body_a = &body_a;
		manifold.body_b = &body_b;
		m_narrow_phase_manifolds.emplace_back(std::move(manifold));
	}
}

void physics_system::narrow_phase_sphere_plane(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	narrow_phase_plane_sphere(body_b, body_a);
}

void physics_system::narrow_phase_sphere_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& collider_a = static_cast<const physics::sphere_collider&>(*body_a.get_collider());
	const auto& collider_b = static_cast<const physics::sphere_collider&>(*body_b.get_collider());
	
	// Transform spheres into world-space
	const fvec3 center_a = body_a.get_transform() * collider_a.get_center();
	const fvec3 center_b = body_b.get_transform() * collider_b.get_center();
	const float radius_a = collider_a.get_radius();
	const float radius_b = collider_b.get_radius();
	
	// Sum sphere radii
	const float sum_radii = radius_a + radius_b;
	
	// Get vector from center a to center b
	const fvec3 difference = center_b - center_a;
	
	const float sqr_distance = sqr_length(difference);
	if (sqr_distance > sum_radii * sum_radii)
	{
		return;
	}
	
	// Ignore degenerate case (sphere centers identical)
	if (!sqr_distance)
	{
		return;
	}
	
	// Init collision manifold
	collision_manifold_type manifold;
	manifold.body_a = &body_a;
	manifold.body_b = &body_b;
	manifold.contact_count = 1;
	
	// Generate collision contact
	auto& contact = manifold.contacts[0];
	
	const float distance = sqrt(sqr_distance);
	
	contact.normal = difference / distance;
	contact.depth = sum_radii - distance;
	contact.point = center_a + contact.normal * (radius_a - contact.depth * 0.5f);
	
	m_narrow_phase_manifolds.emplace_back(std::move(manifold));
}

void physics_system::narrow_phase_sphere_box(physics::rigid_body&, physics::rigid_body&)
{
	return;
}

void physics_system::narrow_phase_sphere_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& collider_a = static_cast<const physics::sphere_collider&>(*body_a.get_collider());
	const auto& collider_b = static_cast<const physics::capsule_collider&>(*body_b.get_collider());
	
	// Transform sphere into world-space
	const fvec3 center_a = body_a.get_transform() * collider_a.get_center();
	const float radius_a = collider_a.get_radius();
	
	// Transform capsule into world-space
	const geom::line_segment<float, 3> segment_b
	{
		body_b.get_transform() * collider_b.get_segment().a,
		body_b.get_transform() * collider_b.get_segment().b
	};
	const float radius_b = collider_b.get_radius();
	
	// Sum the two radii
	const float sum_radii = radius_a + radius_b;
	
	// Find closest point on capsule segment to sphere center
	const auto closest_point = geom::closest_point(segment_b, center_a);
	
	// Get vector from sphere center to point to on capsule segment
	const fvec3 difference = closest_point - center_a;
	
	const float sqr_distance = sqr_length(difference);
	if (sqr_distance > sum_radii * sum_radii)
	{
		return;
	}
	
	// Ignore degenerate case (sphere center on capsule segment)
	if (!sqr_distance)
	{
		return;
	}
	
	collision_manifold_type manifold;
	manifold.contact_count = 1;
	manifold.body_a = &body_a;
	manifold.body_b = &body_b;
	
	auto& contact = manifold.contacts[0];
	
	const float distance = sqrt(sqr_distance);
	
	contact.depth = sum_radii - distance;
	contact.normal = difference / distance;
	contact.point = center_a + contact.normal * (radius_a - contact.depth * 0.5f);
	
	m_narrow_phase_manifolds.emplace_back(std::move(manifold));
}

void physics_system::narrow_phase_box_plane(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	narrow_phase_plane_box(body_b, body_a);
}

void physics_system::narrow_phase_box_sphere(physics::rigid_body&, physics::rigid_body&)
{
	return;
}

void physics_system::narrow_phase_box_box(physics::rigid_body&, physics::rigid_body&)
{
	return;
}

void physics_system::narrow_phase_box_capsule(physics::rigid_body&, physics::rigid_body&)
{
	return;
}

void physics_system::narrow_phase_capsule_plane(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	narrow_phase_plane_capsule(body_b, body_a);
}

void physics_system::narrow_phase_capsule_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	narrow_phase_sphere_capsule(body_b, body_a);
}

void physics_system::narrow_phase_capsule_box(physics::rigid_body&, physics::rigid_body&)
{
	return;
}

void physics_system::narrow_phase_capsule_capsule(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& collider_a = static_cast<const physics::capsule_collider&>(*body_a.get_collider());
	const auto& collider_b = static_cast<const physics::capsule_collider&>(*body_b.get_collider());
	
	// Transform capsules into world-space
	const geom::capsule<float> capsule_a
	{
		{
			body_a.get_transform() * collider_a.get_segment().a,
			body_a.get_transform() * collider_a.get_segment().b
		},
		collider_a.get_radius()
	};
	const geom::capsule<float> capsule_b
	{
		{
			body_b.get_transform() * collider_b.get_segment().a,
			body_b.get_transform() * collider_b.get_segment().b
		},
		collider_b.get_radius()
	};
	
	// Calculate closest points between capsule segments
	const auto [closest_a, closest_b] = geom::closest_point(capsule_a.segment, capsule_b.segment);
	
	// Sum sphere radii
	const float sum_radii = capsule_a.radius + capsule_b.radius;
	
	// Get vector from closest point on segment a to closest point on segment b
	const fvec3 difference = closest_b - closest_a;
	
	const float sqr_distance = sqr_length(difference);
	if (sqr_distance > sum_radii * sum_radii)
	{
		return;
	}
	
	// Ignore degenerate case (closest points identical)
	if (!sqr_distance)
	{
		return;
	}
	
	// Init collision manifold
	collision_manifold_type manifold;
	manifold.body_a = &body_a;
	manifold.body_b = &body_b;
	manifold.contact_count = 1;
	
	// Generate collision contact
	auto& contact = manifold.contacts[0];
	
	const float distance = sqrt(sqr_distance);
	
	contact.normal = difference / distance;
	contact.depth = sum_radii - distance;
	contact.point = closest_a + contact.normal * (capsule_a.radius - contact.depth * 0.5f);
	
	m_narrow_phase_manifolds.emplace_back(std::move(manifold));
}

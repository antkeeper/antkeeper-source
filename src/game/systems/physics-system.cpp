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

#include "game/systems/physics-system.hpp"
#include "game/components/collision-component.hpp"
#include "game/components/rigid-body-component.hpp"
#include "game/components/rigid-body-constraint-component.hpp"
#include "game/components/transform-component.hpp"
#include "game/components/scene-component.hpp"
#include <algorithm>
#include <engine/debug/log.hpp>
#include <engine/entity/id.hpp>
#include <engine/math/transform-operators.hpp>
#include <engine/physics/kinematics/colliders/plane-collider.hpp>
#include <engine/physics/kinematics/colliders/sphere-collider.hpp>
#include <engine/physics/kinematics/colliders/box-collider.hpp>
#include <execution>

namespace {

inline constexpr float coefficient_combine_average(float a, float b) noexcept
{
	return (a + b) * 0.5f;
}

inline constexpr float coefficient_combine_minimum(float a, float b) noexcept
{
	return std::min(a, b);
}

inline constexpr float coefficient_combine_multiply(float a, float b) noexcept
{
	return a * b;
}

inline constexpr float coefficient_combine_maximum(float a, float b) noexcept
{
	return std::max(a, b);
}

} // namespace

physics_system::physics_system(entity::registry& registry):
	updatable_system(registry)
{
	constexpr auto plane_i = std::to_underlying(physics::collider_type::plane);
	constexpr auto sphere_i = std::to_underlying(physics::collider_type::sphere);
	constexpr auto box_i = std::to_underlying(physics::collider_type::box);
	
	narrow_phase_table[plane_i][plane_i] = std::bind_front(&physics_system::narrow_phase_plane_plane, this);
	narrow_phase_table[plane_i][sphere_i] = std::bind(&physics_system::narrow_phase_plane_sphere, this, std::placeholders::_1, std::placeholders::_2, 1.0f);
	narrow_phase_table[plane_i][box_i] = std::bind(&physics_system::narrow_phase_plane_box, this, std::placeholders::_1, std::placeholders::_2, 1.0f);
	
	narrow_phase_table[sphere_i][plane_i] = std::bind_front(&physics_system::narrow_phase_sphere_plane, this);
	narrow_phase_table[sphere_i][sphere_i] = std::bind_front(&physics_system::narrow_phase_sphere_sphere, this);
	narrow_phase_table[sphere_i][box_i] = std::bind_front(&physics_system::narrow_phase_sphere_box, this);
	
	narrow_phase_table[box_i][plane_i] = std::bind_front(&physics_system::narrow_phase_box_plane, this);
	narrow_phase_table[box_i][sphere_i] = std::bind_front(&physics_system::narrow_phase_box_sphere, this);
	narrow_phase_table[box_i][box_i] = std::bind_front(&physics_system::narrow_phase_box_box, this);
}

void physics_system::update(float t, float dt)
{
	detect_collisions_broad();
	detect_collisions_narrow();
	solve_constraints(dt);
	resolve_collisions();
	integrate(dt);
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

void physics_system::interpolate(float alpha)
{
	// Interpolate rigid body states
	auto view = registry.view<rigid_body_component, scene_component>();
	std::for_each
	(
		std::execution::par_unseq,
		view.begin(),
		view.end(),
		[&, alpha](auto entity_id)
		{
			const auto& rigid_body = *(view.get<rigid_body_component>(entity_id).body);
			auto& scene_object = *(view.get<scene_component>(entity_id).object);
			
			scene_object.set_transform(rigid_body.interpolate(alpha));
		}
	);
}

void physics_system::integrate(float dt)
{
	// Drag
	/*
	const float air_density = 1.293f;// * 100.0f;
	const float radius = 1.0f;
	const float sphere_cross_section_area = radius * radius * math::pi<float>;
	const float sphere_drag_coef = 0.47f;
	const float sqr_speed = math::sqr_length(body.linear_velocity);
	if (sqr_speed)
	{
		const float drag_magnitude = -0.5f * air_density * sqr_speed * sphere_drag_coef * sphere_cross_section_area;
		const math::vector<float, 3> drag_force = math::normalize(body.linear_velocity) * drag_magnitude;
		body.apply_central_force(drag_force);
	}
	*/
	
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
			body.apply_central_force(gravity / 10.0f * body.get_mass());
			
			body.integrate(dt);
		}
	);
}

void physics_system::solve_constraints(float dt)
{
	registry.view<rigid_body_constraint_component>().each
	(
		[dt](auto& component)
		{
			component.constraint->solve(dt);
		}
	);
}

void physics_system::detect_collisions_broad()
{
	broad_phase_pairs.clear();
	
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
			
			broad_phase_pairs.emplace_back(&body_a, &body_b);
		}
	}
}

void physics_system::detect_collisions_narrow()
{
	narrow_phase_manifolds.clear();
	
	for (const auto& pair: broad_phase_pairs)
	{
		auto& body_a = *pair.first;
		auto& body_b = *pair.second;
		
		narrow_phase_table[std::to_underlying(body_a.get_collider()->type())][std::to_underlying(body_b.get_collider()->type())](body_a, body_b);
	}
}

void physics_system::resolve_collisions()
{
	for (const auto& manifold: narrow_phase_manifolds)
	{
		auto& body_a = *manifold.body_a;
		auto& body_b = *manifold.body_b;
		
		const auto& material_a = *body_a.get_collider()->get_material();
		const auto& material_b = *body_b.get_collider()->get_material();
		
		// Calculate coefficient of restitution
		const auto restitution_combine = std::max(material_a.get_restitution_combine_mode(), material_b.get_restitution_combine_mode());
		float restitution_coef{0.0f};
		switch (restitution_combine)
		{
			case physics::restitution_combine_mode::average:
				restitution_coef = coefficient_combine_average(material_a.get_restitution(), material_b.get_restitution());
				break;
			
			case physics::restitution_combine_mode::minimum:
				restitution_coef = coefficient_combine_minimum(material_a.get_restitution(), material_b.get_restitution());
				break;
			
			case physics::restitution_combine_mode::multiply:
				restitution_coef = coefficient_combine_multiply(material_a.get_restitution(), material_b.get_restitution());
				break;
			
			case physics::restitution_combine_mode::maximum:
				restitution_coef = coefficient_combine_maximum(material_a.get_restitution(), material_b.get_restitution());
				break;
			
			default:
				break;
		}
		
		// Calculate coefficients of friction
		const auto friction_combine = std::max(material_a.get_friction_combine_mode(), material_b.get_friction_combine_mode());
		float static_friction_coef{0.0f};
		float dynamic_friction_coef{0.0f};
		switch (restitution_combine)
		{
			case physics::restitution_combine_mode::average:
				static_friction_coef = coefficient_combine_average(material_a.get_static_friction(), material_b.get_static_friction());
				dynamic_friction_coef = coefficient_combine_average(material_a.get_dynamic_friction(), material_b.get_dynamic_friction());
				break;
			
			case physics::restitution_combine_mode::minimum:
				static_friction_coef = coefficient_combine_minimum(material_a.get_static_friction(), material_b.get_static_friction());
				dynamic_friction_coef = coefficient_combine_minimum(material_a.get_dynamic_friction(), material_b.get_dynamic_friction());
				break;
			
			case physics::restitution_combine_mode::multiply:
				static_friction_coef = coefficient_combine_multiply(material_a.get_static_friction(), material_b.get_static_friction());
				dynamic_friction_coef = coefficient_combine_multiply(material_a.get_dynamic_friction(), material_b.get_dynamic_friction());
				break;
			
			case physics::restitution_combine_mode::maximum:
				static_friction_coef = coefficient_combine_maximum(material_a.get_static_friction(), material_b.get_static_friction());
				dynamic_friction_coef = coefficient_combine_maximum(material_a.get_dynamic_friction(), material_b.get_dynamic_friction());
				break;
			
			default:
				break;
		}
		
		const float sum_inverse_mass = body_a.get_inverse_mass() + body_b.get_inverse_mass();
		const float impulse_scale = 1.0f / static_cast<float>(manifold.contact_count);
		
		for (std::uint8_t i = 0; i < manifold.contact_count; ++i)
		{
			const physics::collision_contact& contact = manifold.contacts[i];
			
			const math::vector<float, 3> radius_a = contact.point - body_a.get_position();
			const math::vector<float, 3> radius_b = contact.point - body_b.get_position();
			
			math::vector<float, 3> relative_velocity = body_b.get_point_velocity(radius_b) - body_a.get_point_velocity(radius_a);
			
			const float contact_velocity = math::dot(relative_velocity, contact.normal);
			if (contact_velocity > 0.0f)
			{
				continue;
			}
			
			const float reaction_impulse_num = -(1.0f + restitution_coef) * contact_velocity;
			const math::vector<float, 3> ra_cross_n = math::cross(radius_a, contact.normal);
			const math::vector<float, 3> rb_cross_n = math::cross(radius_b, contact.normal);
			const float reaction_impulse_den = sum_inverse_mass +
				math::dot
				(
					math::cross(body_a.get_inverse_inertia() * ra_cross_n, radius_a) +
					math::cross(body_b.get_inverse_inertia() * rb_cross_n, radius_b),
					contact.normal
				);
			const float reaction_impulse_mag = (reaction_impulse_num / reaction_impulse_den) * impulse_scale;
			const math::vector<float, 3> reaction_impulse = contact.normal * reaction_impulse_mag;
			
			// Apply reaction impulses
			body_a.apply_impulse(-reaction_impulse, radius_a);
			body_b.apply_impulse(reaction_impulse, radius_b);
			
			//relative_velocity = body_b.get_point_velocity(radius_b) - body_a.get_point_velocity(radius_a);
			
			math::vector<float, 3> contact_tangent = relative_velocity - contact.normal * contact_velocity;
			const float sqr_tangent_length = math::sqr_length(contact_tangent);
			if (sqr_tangent_length > 0.0f)
			{
				contact_tangent /= std::sqrt(sqr_tangent_length);
			}
					
			const float friction_impulse_num = math::dot(relative_velocity, -contact_tangent);
			const math::vector<float, 3> ra_cross_t = math::cross(radius_a, contact_tangent);
			const math::vector<float, 3> rb_cross_t = math::cross(radius_b, contact_tangent);
			const float friction_impulse_den = sum_inverse_mass +
				math::dot
				(
					math::cross(body_a.get_inverse_inertia() * ra_cross_t, radius_a) +
					math::cross(body_b.get_inverse_inertia() * rb_cross_t, radius_b),
					contact_tangent
				);
			float friction_impulse_mag = (friction_impulse_num / friction_impulse_den) * impulse_scale;
			
			if (std::abs(friction_impulse_mag) >= reaction_impulse_mag * static_friction_coef)
			{
				friction_impulse_mag = -reaction_impulse_mag * dynamic_friction_coef;
			}
			
			const math::vector<float, 3> friction_impulse = contact_tangent * friction_impulse_mag;
			
			body_a.apply_impulse(-friction_impulse, radius_a);
			body_b.apply_impulse(friction_impulse, radius_b);
		}
	}
}

void physics_system::correct_positions()
{
	const float depth_threshold = 0.01f;
	const float correction_factor = 0.4f;
	
	for (const auto& manifold: narrow_phase_manifolds)
	{
		auto& body_a = *manifold.body_a;
		auto& body_b = *manifold.body_b;
		const float sum_inverse_mass = body_a.get_inverse_mass() + body_b.get_inverse_mass();
		
		for (std::uint8_t i = 0; i < manifold.contact_count; ++i)
		{
			const physics::collision_contact& contact = manifold.contacts[i];
			
			math::vector<float, 3> correction = contact.normal * (std::max<float>(0.0f, contact.depth - depth_threshold) / sum_inverse_mass) * correction_factor;
			
			body_a.set_position(body_a.get_position() - correction * body_a.get_inverse_mass());
			body_b.set_position(body_b.get_position() + correction * body_b.get_inverse_mass());
		}
	}
}

void physics_system::narrow_phase_plane_plane(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	return;
}

void physics_system::narrow_phase_plane_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b, float normal_sign)
{
	const auto& plane_a = static_cast<const physics::plane_collider&>(*body_a.get_collider());
	const auto& sphere_b = static_cast<const physics::sphere_collider&>(*body_b.get_collider());
	
	// Transform plane into world-space
	const math::vector<float, 3> plane_normal = body_a.get_orientation() * plane_a.get_normal();
	const float plane_constant = plane_a.get_constant() - math::dot(plane_normal, body_a.get_position());
	
	const float signed_distance = math::dot(plane_normal, body_b.get_position()) + plane_constant;
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
	contact.normal = plane_normal * -normal_sign;
	contact.depth = std::abs(signed_distance - sphere_b.get_radius());
	
	narrow_phase_manifolds.emplace_back(std::move(manifold));
}

void physics_system::narrow_phase_plane_box(physics::rigid_body& body_a, physics::rigid_body& body_b, float normal_sign)
{
	const auto& plane_a = static_cast<const physics::plane_collider&>(*body_a.get_collider());
	const auto& box_b = static_cast<const physics::box_collider&>(*body_b.get_collider());
	
	// Transform plane into world-space
	const math::vector<float, 3> plane_normal = body_a.get_orientation() * plane_a.get_normal();
	const float plane_constant = plane_a.get_constant() - math::dot(plane_normal, body_a.get_position());
	
	const auto& box_min = box_b.get_min();
	const auto& box_max = box_b.get_max();
	const math::vector<float, 3> corners[8] =
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
	for (std::size_t i = 0; i < 8; ++i)
	{
		// Transform corner into world-space
		const math::vector<float, 3> point = body_b.get_transform() * corners[i];
		
		const float signed_distance = math::dot(plane_normal, point) + plane_constant;
		
		if (signed_distance <= 0.0f)
		{
			auto& contact = manifold.contacts[manifold.contact_count];
			contact.point = point;
			contact.normal = plane_normal * -normal_sign;
			contact.depth = std::abs(signed_distance);
			
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
		narrow_phase_manifolds.emplace_back(std::move(manifold));
	}
}

void physics_system::narrow_phase_sphere_plane(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	narrow_phase_plane_sphere(body_b, body_a, -1.0f);
}

void physics_system::narrow_phase_sphere_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	const auto& sphere_a = static_cast<const physics::sphere_collider&>(*body_a.get_collider());
	const auto& sphere_b = static_cast<const physics::sphere_collider&>(*body_b.get_collider());
	
	const float sum_radii = sphere_a.get_radius() + sphere_b.get_radius();
	const float sqr_sum_radii = sum_radii * sum_radii;
	
	const math::vector<float, 3> difference = body_b.get_position() - body_a.get_position();
	
	const float sqr_distance = math::dot(difference, difference);
	if (sqr_distance > sqr_sum_radii)
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
	if (sqr_distance != 0.0f)
	{
		const float distance = std::sqrt(sqr_distance);
		contact.normal = difference / distance;
		contact.depth = sum_radii - distance;
	}
	else
	{
		contact.normal = {1.0f, 0.0f, 0.0f};
		contact.depth = sum_radii;
	}
	contact.point = body_a.get_position() + contact.normal * sphere_a.get_radius();
	
	narrow_phase_manifolds.emplace_back(std::move(manifold));
}

void physics_system::narrow_phase_sphere_box(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	return;
}

void physics_system::narrow_phase_box_plane(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	narrow_phase_plane_box(body_b, body_a, -1.0f);
}

void physics_system::narrow_phase_box_sphere(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	return;
}

void physics_system::narrow_phase_box_box(physics::rigid_body& body_a, physics::rigid_body& body_b)
{
	return;
}

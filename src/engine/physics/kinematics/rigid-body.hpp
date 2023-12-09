// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_PHYSICS_RIGID_BODY_HPP
#define ANTKEEPER_PHYSICS_RIGID_BODY_HPP

#include <engine/math/vector.hpp>
#include <engine/math/quaternion.hpp>
#include <engine/physics/kinematics/collider.hpp>
#include <engine/math/transform.hpp>
#include <memory>

namespace physics {

/**
 * Rigid body.
 */
class rigid_body
{
public:
	/**
	 * Sets the transformation representing the current state of the rigid body.
	 *
	 * @param transform Transformation representing the current state of the rigid body.
	 */
	inline constexpr void set_transform(const math::transform<float>& transform) noexcept
	{
		m_current_transform = transform;
	}
	
	/**
	 * Sets the current position of the rigid body.
	 *
	 * @param position Position of the rigid body.
	 */
	inline constexpr void set_position(const math::fvec3& position) noexcept
	{
		m_current_transform.translation = position;
	}
	
	/**
	 * Sets the current orientation of the rigid body.
	 *
	 * @param orientation Orientation of the rigid body.
	 */
	inline constexpr void set_orientation(const math::fquat& orientation) noexcept
	{
		m_current_transform.rotation = orientation;
	}
	
	/**
	 * Sets the current scale of the rigid body.
	 *
	 * @param scale Scale of the rigid body.
	 */
	/// @{
	inline constexpr void set_scale(const math::fvec3& scale) noexcept
	{
		m_current_transform.scale = scale;
	}
	inline constexpr void set_scale(float scale) noexcept
	{
		m_current_transform.scale = {scale, scale, scale};
	}
	/// @}
	
	/**
	 * Sets the transformation representing the previous state of the rigid body.
	 *
	 * @param transform Transformation representing the previous state of the rigid body.
	 */
	inline constexpr void set_previous_transform(const math::transform<float>& transform) noexcept
	{
		m_previous_transform = transform;
	}
	
	/**
	 * Sets the previous position of the rigid body.
	 *
	 * @param position Position of the rigid body.
	 */
	inline constexpr void set_previous_position(const math::fvec3& position) noexcept
	{
		m_previous_transform.translation = position;
	}
	
	/**
	 * Sets the previous orientation of the rigid body.
	 *
	 * @param orientation Orientation of the rigid body.
	 */
	inline constexpr void set_previous_orientation(const math::fquat& orientation) noexcept
	{
		m_previous_transform.rotation = orientation;
	}
	
	/**
	 * Sets the previous scale of the rigid body.
	 *
	 * @param scale Scale of the rigid body.
	 */
	/// @{
	inline constexpr void set_previous_scale(const math::fvec3& scale) noexcept
	{
		m_previous_transform.scale = scale;
	}
	inline constexpr void set_previous_scale(float scale) noexcept
	{
		m_previous_transform.scale = {scale, scale, scale};
	}
	/// @}
	
	/**
	 * Sets the center of mass of the rigid body.
	 *
	 * @param point World-space center of mass.
	 */
	inline constexpr void set_center_of_mass(const math::fvec3& point) noexcept
	{
		m_center_of_mass = point;
	}
	
	/**
	 * Sets mass of the rigid body.
	 *
	 * @param mass Mass, in kg.
	 */
	inline constexpr void set_mass(float mass) noexcept
	{
		m_mass = mass;
		m_inverse_mass = (mass) ? 1.0f / mass : 0.0f;
	}
	
	/**
	 * Sets the moment of inertia of the rigid body.
	 *
	 * @param inertia Moment of inertia, in kg⋅m^2.
	 */
	inline constexpr void set_inertia(float inertia) noexcept
	{
		m_inertia = inertia;
		m_inverse_inertia = (inertia) ? 1.0f / inertia : 0.0f;
	}
	
	/**
	 * Sets the collider of the rigid body.
	 *
	 * @param collider Shared pointer to a collider.
	 */
	inline void set_collider(std::shared_ptr<collider> collider) noexcept
	{
		m_collider = collider;
	}
	
	/**
	 * Sets the linear damping factor of the rigid body.
	 *
	 * @param damping Linear damping factor.
	 */
	inline constexpr void set_linear_damping(float damping) noexcept
	{
		m_linear_damping = damping;
	}
	
	/**
	 * Sets the angular damping factor of the rigid body.
	 *
	 * @param damping Angular damping factor.
	 */
	inline constexpr void set_angular_damping(float damping) noexcept
	{
		m_angular_damping = damping;
	}
	
	/**
	 * Sets the linear momentum of the rigid body.
	 *
	 * @param momentum Linear momentum, in kg⋅m/s.
	 */
	inline constexpr void set_linear_momentum(const math::fvec3& momentum) noexcept
	{
		m_linear_momentum = momentum;
		m_linear_velocity = m_inverse_mass * m_linear_momentum;
	}
	
	/**
	 * Sets the angular momentum of the rigid body.
	 *
	 * @param momentum Angular momentum, in kg⋅m^2⋅s^-1.
	 */
	inline constexpr void set_angular_momentum(const math::fvec3& momentum) noexcept
	{
		m_angular_momentum = momentum;
		m_angular_velocity = m_inverse_inertia * m_angular_momentum;
	}
	
	/**
	 * Sets the linear velocity of the rigid body.
	 *
	 * @param velocity Linear velocity, in m/s.
	 */
	inline constexpr void set_linear_velocity(const math::fvec3& velocity) noexcept
	{
		m_linear_velocity = velocity;
		m_linear_momentum = m_mass * m_linear_velocity;
	}
	
	/**
	 * Sets the angular velocity of the rigid body.
	 *
	 * @param velocity Angular velocity, rad/s.
	 */
	inline constexpr void set_angular_velocity(const math::fvec3& velocity) noexcept
	{
		m_angular_velocity = velocity;
		m_angular_momentum = m_inertia * m_angular_velocity;
	}
	
	/// Returns the transformation representing the current state of the rigid body.
	[[nodiscard]] inline constexpr const math::transform<float>& get_transform() const noexcept
	{
		return m_current_transform;
	}
	
	/// Returns the current position of the rigid body.
	[[nodiscard]] inline constexpr const math::fvec3& get_position() const noexcept
	{
		return m_current_transform.translation;
	}
	
	/// Returns the current orientation of the rigid body.
	[[nodiscard]] inline constexpr const math::fquat& get_orientation() const noexcept
	{
		return m_current_transform.rotation;
	}
	
	/// Returns the current scale of the rigid body.
	[[nodiscard]] inline constexpr const math::fvec3& get_scale() const noexcept
	{
		return m_current_transform.scale;
	}
	
	/// Returns the transformation representing the previous state of the rigid body.
	[[nodiscard]] inline constexpr const math::transform<float>& get_previous_transform() const noexcept
	{
		return m_previous_transform;
	}
	
	/// Returns the previous position of the rigid body.
	[[nodiscard]] inline constexpr const math::fvec3& get_previous_position() const noexcept
	{
		return m_previous_transform.translation;
	}
	
	/// Returns the previous orientation of the rigid body.
	[[nodiscard]] inline constexpr const math::fquat& get_previous_orientation() const noexcept
	{
		return m_previous_transform.rotation;
	}
	
	/// Returns the previous scale of the rigid body.
	[[nodiscard]] inline constexpr const math::fvec3& get_previous_scale() const noexcept
	{
		return m_previous_transform.scale;
	}
	
	/// Returns the center of mass of the rigid body.
	[[nodiscard]] inline constexpr const math::fvec3& get_center_of_mass() const noexcept
	{
		return m_center_of_mass;
	}
	
	/// Returns the mass of the rigid body, in kg.
	[[nodiscard]] inline constexpr float get_mass() const noexcept
	{
		return m_mass;
	}
	
	/// Returns the inverse mass of the rigid body, in kg^-1.
	[[nodiscard]] inline constexpr float get_inverse_mass() const noexcept
	{
		return m_inverse_mass;
	}
	
	/// Returns the moment of inertia of the rigid body, in kg⋅m^2.
	[[nodiscard]] inline constexpr float get_inertia() const noexcept
	{
		return m_inertia;
	}
	
	/// Returns the inverse moment of inertia of the rigid body, in kg⋅m^2^-1.
	[[nodiscard]] inline constexpr float get_inverse_inertia() const noexcept
	{
		return m_inverse_inertia;
	}
	
	/// Returns the linear damping factor of the rigid body.
	[[nodiscard]] inline constexpr float get_linear_damping() const noexcept
	{
		return m_linear_damping;
	}
	
	/// Returns the angular damping factor of the rigid body.
	[[nodiscard]] inline constexpr float get_angular_damping() const noexcept
	{
		return m_angular_damping;
	}
	
	/// Returns the collider of the rigid body.
	[[nodiscard]] inline constexpr const std::shared_ptr<collider>& get_collider() const noexcept
	{
		return m_collider;
	}
	
	/// Returns the linear momentum of the rigid body, in kg⋅m/s.
	[[nodiscard]] inline constexpr const math::fvec3& get_linear_momentum() const noexcept
	{
		return m_linear_momentum;
	}
	
	/// Returns the angular momentum of the rigid body, in kg⋅m^2⋅s^-1.
	[[nodiscard]] inline constexpr const math::fvec3& get_angular_momentum() const noexcept
	{
		return m_angular_momentum;
	}
	
	/// Returns the linear velocity of the rigid body, in m/s.
	[[nodiscard]] inline constexpr const math::fvec3& get_linear_velocity() const noexcept
	{
		return m_linear_velocity;
	}
	
	/// Returns the angular velocity of the rigid body, in rad/s.
	[[nodiscard]] inline constexpr const math::fvec3& get_angular_velocity() const noexcept
	{
		return m_angular_velocity;
	}
	
	/// Returns the total pre-integrated force, in N.
	[[nodiscard]] inline constexpr const math::fvec3& get_applied_force() const noexcept
	{
		return m_applied_force;
	}
	
	/// Returns the total pre-integrated torque, in N⋅m.
	[[nodiscard]] inline constexpr const math::fvec3& get_applied_torque() const noexcept
	{
		return m_applied_torque;
	}
	
	/**
	 * Calculates the total velocity at a point on the rigid body.
	 *
	 * @param radius Radius vector from the center of mass to the point at which the velocity should be calculated.
	 *
	 * @return Point velocity.
	 */
	[[nodiscard]] inline constexpr math::fvec3 get_point_velocity(const math::fvec3& radius) const noexcept
	{
		return m_linear_velocity + math::cross(m_angular_velocity, radius);
	}
	
	/**
	 * Returns `true` if the rigid body is static, `false` otherwise.
	 */
	[[nodiscard]] inline constexpr bool is_static() const noexcept
	{
		return (m_mass == 0.0f);
	}
	
	/**
	 * Applies a force at a point on the rigid body.
	 *
	 * @param force Force to apply, in N.
	 * @param radius Radius vector from the center of mass to the point at which the force should be applied.
	 */
	inline constexpr void apply_force(const math::fvec3& force, const math::fvec3& radius) noexcept
	{
		m_applied_force += force;
		m_applied_torque += math::cross(radius, force);
	}
	
	/**
	 * Applies a force at the center of mass of the rigid body.
	 *
	 * @param force Force to apply, in N.
	 */
	inline constexpr void apply_central_force(const math::fvec3& force) noexcept
	{
		m_applied_force += force;
	}
	
	/**
	 * Applies a torque to the rigid body.
	 *
	 * @param torque Torque to apply.
	 */
	inline constexpr void apply_torque(const math::fvec3& torque) noexcept
	{
		m_applied_torque += torque;
	}
	
	/**
	 * Applies an impulse at a point on the rigid body.
	 *
	 * @param impulse Impulse to apply, in N⋅s.
	 * @param radius Radius vector from the center of mass to the point at which the impulse should be applied.
	 */
	inline constexpr void apply_impulse(const math::fvec3& impulse, const math::fvec3& radius) noexcept
	{
		m_linear_momentum += impulse;
		m_angular_momentum += math::cross(radius, impulse);
		
		// Update velocities
		m_linear_velocity = m_inverse_mass * m_linear_momentum;
		m_angular_velocity = m_inverse_inertia * m_angular_momentum;
	}
	
	/**
	 * Applies an impulse at the center of mass of the rigid body.
	 *
	 * @param impulse Impulse to apply, in N⋅s.
	 */
	inline constexpr void apply_central_impulse(const math::fvec3& impulse) noexcept
	{
		m_linear_momentum += impulse;
		
		// Update linear velocity
		m_linear_velocity = m_inverse_mass * m_linear_momentum;
	}
	
	/**
	 * Applies a torque impulse to the rigid body.
	 *
	 * @param torque Torque impulse to apply.
	 */
	inline constexpr void apply_torque_impulse(const math::fvec3& torque) noexcept
	{
		m_angular_momentum += torque;
		
		// Update angular velocity
		m_angular_velocity = m_inverse_inertia * m_angular_momentum;
	}
	
	/// Clears all pre-integrated forces.
	inline constexpr void clear_applied_forces() noexcept
	{
		m_applied_force = {};
		m_applied_torque = {};
	}
	
	/**
	 * Integrates forces, updating the momentums and velocities of the rigid body.
	 *
	 * @param dt Timestep, in seconds.
	 */
	void integrate_forces(float dt) noexcept;
	
	/**
	 * Integrates velocities, updating the center of mass and orientation of the rigid body.
	 *
	 * @param dt Timestep, in seconds.
	 */
	void integrate_velocities(float dt) noexcept;
	
	/**
	 * Integrates forces and velocities.
	 *
	 * @param dt Timestep, in seconds.
	 */
	inline void integrate(float dt) noexcept
	{
		integrate_forces(dt);
		integrate_velocities(dt);
	}
	
	/**
	 * Returns a transformation representing a state of the rigid body between its current and previous states.
	 *
	 * @param alpha State interpolation factor.
	 *
	 * @return Interpolated transformation.
	 */
	[[nodiscard]] math::transform<float> interpolate(float alpha) const;
	
private:
	/// Transformation representing the current state of the rigid body.
	math::transform<float> m_current_transform{math::identity<math::transform<float>>};
	
	/// Transformation representing the previous state of the rigid body.
	math::transform<float> m_previous_transform{math::identity<math::transform<float>>};
	
	/// Center of mass.
	math::fvec3 m_center_of_mass{};
	
	/// Mass, in kg.
	float m_mass{1.0f};
	
	/// Inverse mass, in kg^-1.
	float m_inverse_mass{1.0f};
	
	/// Moment of inertia, in kg⋅m^2.
	float m_inertia{1.0f};
	
	/// Inverse moment of inertia, in kg⋅m^2^-1.
	float m_inverse_inertia{1.0f};
	
	/// Linear damping factor.
	float m_linear_damping{};
	
	/// Angular damping factor.
	float m_angular_damping{};
	
	/// Collider object.
	std::shared_ptr<collider> m_collider;
	
	/// Linear momentum, in kg⋅m/s.
	math::fvec3 m_linear_momentum{};
	
	/// Angular momentum, in kg⋅m^2⋅s^-1.
	math::fvec3 m_angular_momentum{};
	
	/// Linear velocity, in m/s.
	math::fvec3 m_linear_velocity{};
	
	/// Angular velocity, in rad/s.
	math::fvec3 m_angular_velocity{};
	
	/// Applied force, in N.
	math::fvec3 m_applied_force{};
	
	/// Applied torque, in N⋅m.
	math::fvec3 m_applied_torque{};
};

} // namespace physics

#endif // ANTKEEPER_PHYSICS_RIGID_BODY_HPP

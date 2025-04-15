// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.kinematics.spring_constraint;
export import engine.physics.kinematics.constraint;
export import engine.physics.kinematics.rigid_body;
export import engine.math.vector;

export namespace engine::physics
{
	/// Spring constraint.
	class spring_constraint: public constraint
	{
	public:
		/// Destructs a spring constraint.
		~spring_constraint() override = default;

		/// Solves the spring constraint.
		void solve(float dt) override;

		/// Attaches the spring to body a.
		/// @param body_a Body to which the spring should be attached.
		/// @param point_a Point on body a, in body-space, at which the spring should be attached.
		inline constexpr void attach_a(rigid_body& body_a, const math::fvec3& point_a) noexcept
		{
			m_body_a = &body_a;
			m_point_a = point_a;
		}

		/// Attaches the spring to body b.
		/// @param body_b Body to which the spring should be attached.
		/// @param point_b Point on body b, in body-space, at which the spring should be attached.
		inline constexpr void attach_b(rigid_body& body_b, const math::fvec3& point_b) noexcept
		{
			m_body_b = &body_b;
			m_point_b = point_b;
		}

		/// Detaches the spring from body a.
		inline constexpr void detach_a() noexcept
		{
			m_body_a = nullptr;
		}

		/// Detaches the spring from body b.
		inline constexpr void detach_b() noexcept
		{
			m_body_b = nullptr;
		}

		/// Detaches the spring from bodies a and b.
		inline constexpr void detach() noexcept
		{
			detach_a();
			detach_b();
		}

		/// Sets the resting length of the spring.
		/// @param length Resting length, in meters.
		inline constexpr void set_resting_length(float length) noexcept
		{
			m_resting_length = length;
		}

		/// Sets the stiffness constant of the spring.
		/// @param stiffness Stiffness constant.
		inline constexpr void set_stiffness(float stiffness) noexcept
		{
			m_stiffness = stiffness;
		}

		/// Sets the damping constant of the spring.
		/// @param damping Damping constant.
		inline constexpr void set_damping(float damping) noexcept
		{
			m_damping = damping;
		}

		/// Returns the body to which the spring is attached at point a.
		[[nodiscard]] inline constexpr rigid_body* get_body_a() const noexcept
		{
			return m_body_a;
		}

		/// Returns the body to which the spring is attached at point b.
		[[nodiscard]] inline constexpr rigid_body* get_body_b() const noexcept
		{
			return m_body_b;
		}

		/// Returns the point at which the spring is attached to body a, in body-space.
		[[nodiscard]] inline constexpr const math::fvec3& get_point_a() const noexcept
		{
			return m_point_a;
		}

		/// Returns the point at which the spring is attached to body b, in body-space.
		[[nodiscard]] inline constexpr const math::fvec3& get_point_b() const noexcept
		{
			return m_point_b;
		}

		/// Returns the resting length of the spring, in meters.
		[[nodiscard]] inline constexpr float get_resting_length() const noexcept
		{
			return m_resting_length;
		}

		/// Returns the stiffness constant of the spring.
		[[nodiscard]] inline constexpr float get_stiffness() const noexcept
		{
			return m_stiffness;
		}

		/// Returns the damping constant of the spring.
		[[nodiscard]] inline constexpr float get_damping() const noexcept
		{
			return m_damping;
		}

	private:
		/// Rigid body to which the spring is attached at point a.
		rigid_body* m_body_a{};

		/// Rigid body to which the spring is attached at point b.
		rigid_body* m_body_b{};

		/// Point at which the spring is attached to body a, in body-space.
		math::fvec3 m_point_a{};

		/// Point at which the spring is attached to body b, in body-space.
		math::fvec3 m_point_b{};

		/// Resting length of the spring, in meters.
		float m_resting_length{};

		/// Stiffness constant of the spring.
		float m_stiffness{1.0f};

		/// Damping constant of the spring.
		float m_damping{1.0f};
	};
}

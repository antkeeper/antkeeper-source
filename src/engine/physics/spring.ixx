// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.physics.spring;
import engine.physics.frequency;
import engine.math.constants;

export namespace engine::physics
{
	/// Solves a numeric spring using the implicit Euler method.
	/// @tparam T Value type.
	/// @tparam S Scalar type.
	/// @param[in,out] x Current value of the spring.
	/// @param[in,out] v Current velocity of the spring.
	/// @param[in] xt Target value of the spring.
	/// @param[in] z Damping ratio of the spring, which can be undamped (`0`), underdamped (`< 1`), critically damped (`1`), or overdamped (`> 1`).	
	/// @param[in] w Angular frequency of the spring oscillation, in radians per second.
	/// @param[in] dt Delta time, in seconds.
	template <class T, class S>
	constexpr void solve_spring(T& x, T& v, const T& xt, S z, S w, S dt) noexcept
	{
		const auto ww_dt = w * w * dt;
		const auto ww_dtdt = ww_dt * dt;
		const auto f = z * w * dt * S{2} + S{1};
		const auto inv_det = S{1} / (f + ww_dtdt);

		x = (x * f + v * dt + xt * ww_dtdt) * inv_det;
		v = (v + (xt - x) * ww_dt) * inv_det;
	}

	/// Numeric spring.
	/// @tparam T Value type.
	/// @tparam S Scalar type.
	template <class T, class S>
	class numeric_spring
	{
	public:
		/// Value type.
		using value_type = T;

		/// Scalar type.
		using scalar_type = S;

		/// Solves the spring using the implicit Euler method.
		/// @param dt Delta time, in seconds.
		inline constexpr void solve(scalar_type dt) noexcept
		{
			solve_spring(m_value, m_velocity, m_target_value, m_damping_ratio, m_angular_frequency, dt);
		}

		/// Sets the current value of the spring.
		/// @param value Current value.
		inline constexpr void set_value(const value_type& value) noexcept
		{
			m_value = value;
		}

		/// Sets the target value of the spring.
		/// @param value Target value.
		inline constexpr void set_target_value(const value_type& value) noexcept
		{
			m_target_value = value;
		}

		/// Sets the velocity of the spring.
		/// @param velocity Spring velocity.
		inline constexpr void set_velocity(const value_type& velocity) noexcept
		{
			m_velocity = velocity;
		}

		/// Sets the damping ratio of the spring.
		/// @param ratio Damping ratio, which can be undamped (`0`), underdamped (`< 1`), critically damped (`1`), or overdamped (`> 1`).
		inline constexpr void set_damping_ratio(scalar_type ratio) noexcept
		{
			m_damping_ratio = ratio;
		}

		/// Sets the angular frequency of the spring oscillation.
		/// @param angular_frequency Angular frequency, in radians per second.
		inline constexpr void set_angular_frequency(scalar_type angular_frequency) noexcept
		{
			m_angular_frequency = angular_frequency;
		}

		/// Sets the oscillation period of the spring.
		/// @param period Oscillation period, in seconds.
		inline constexpr void set_period(scalar_type period) noexcept
		{
			m_angular_frequency = s_to_rads(period);
		}

		/// Sets the oscillation frequency of the spring.
		/// @param frequency Oscillation frequency, in hertz.
		inline constexpr void set_frequency(scalar_type frequency) noexcept
		{
			m_angular_frequency = hz_to_rads(frequency);
		}

		/// Returns the current value of the spring.
		[[nodiscard]] inline constexpr const value_type& get_value() const noexcept
		{
			return m_value;
		}

		/// Returns the target value of the spring.
		[[nodiscard]] inline constexpr const value_type& get_target_value() const noexcept
		{
			return m_target_value;
		}

		/// Returns the velocity of the spring.
		[[nodiscard]] inline constexpr const value_type& get_velocity() const noexcept
		{
			return m_velocity;
		}

		/// Returns the damping ratio of the spring.
		[[nodiscard]] inline constexpr scalar_type get_damping_ratio() const noexcept
		{
			return m_damping_ratio;
		}

		/// Returns the angular frequency of the spring oscillation, in radians per second.
		[[nodiscard]] inline constexpr scalar_type get_angular_frequency() const noexcept
		{
			return m_angular_frequency;
		}

		/// Returns the oscillation period of the spring, in seconds.
		[[nodiscard]] inline constexpr scalar_type get_period() const noexcept
		{
			return rads_to_s(m_angular_frequency);
		}

		/// Returns the oscillation frequency of the spring, in hertz.
		[[nodiscard]] inline constexpr scalar_type get_frequency() const noexcept
		{
			return rads_to_hz(m_angular_frequency);
		}

	private:
		value_type m_value{};
		value_type m_target_value{};
		value_type m_velocity{};
		scalar_type m_damping_ratio{1};
		scalar_type m_angular_frequency{math::two_pi<scalar_type>};
	};
}

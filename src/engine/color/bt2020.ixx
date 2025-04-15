// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.bt2020;
export import engine.color.rgb;
export import engine.math.vector;
import engine.math.functions;

export namespace engine::color
{
	/// @name ITU-R BT.2020 color space
	/// @{

	/// ITU-R BT.2020 Opto-Electronic Transfer Function (OETF).
	/// @tparam T Scalar type.
	/// @param x Linear BT.2020 tristimulus.
	/// @param alpha OETF alpha constant.
	/// @param beta OETF beta constant.
	/// @return Non-linear BT.2020 signal.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_oetf(const math::vec3<T>& x, T alpha, T beta)
	{
		auto f = [alpha, beta](T x) -> T
		{
			return x < beta ? T{4.5} * x : alpha * math::pow(x, T{0.45}) - (alpha - T{1});
		};
	
		return {f(x[0]), f(x[1]), f(x[2])};
	}

	/// ITU-R BT.2020 precise Opto-Electronic Transfer Function (OETF).
	/// @tparam T Scalar type.
	/// @param x Linear BT.2020 tristimulus.
	/// @return Non-linear BT.2020 signal.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_oetf_precise(const math::vec3<T>& x)
	{
		return bt2020_oetf(x, T{1} + T{5.5} * T{0.018053968510807}, T{0.018053968510807});
	}

	/// ITU-R BT.2020 12-bit Opto-Electronic Transfer Function (OETF).
	/// @tparam T Scalar type.
	/// @param x Linear BT.2020 tristimulus.
	/// @return Non-linear BT.2020 signal.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_oetf_12_bits(const math::vec3<T>& x)
	{
		return bt2020_oetf(x, T{1.0993}, T{0.0181});
	}

	/// ITU-R BT.2020 10-bit Opto-Electronic Transfer Function (OETF).
	/// @tparam T Scalar type.
	/// @param x Linear BT.2020 tristimulus.
	/// @return Non-linear BT.2020 signal.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_oetf_10_bits(const math::vec3<T>& x)
	{
		return bt2020_oetf(x, T{1.099}, T{0.018});
	}

	/// ITU-R BT.2020 inverse OETF.
	/// @tparam T Scalar type.
	/// @param x Non-linear BT.2020 signal.
	/// @param alpha OETF alpha constant.
	/// @param beta OETF beta constant.
	/// @return Linear BT.2020 tristimulus.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_inverse_oetf(const math::vec3<T>& x, T alpha, T beta)
	{
		const auto oetf_beta = alpha * math::pow(beta, T{0.45}) - (alpha - T{1});

		auto f = [alpha, oetf_beta](T x) -> T
		{
			return x < oetf_beta ? x / T{4.5} : math::pow((x + alpha - T{1}) / alpha, T{1} / T{0.45});
		};
	
		return {f(x[0]), f(x[1]), f(x[2])};
	}

	/// ITU-R BT.2020 precise inverse OETF.
	/// @tparam T Scalar type.
	/// @param x Non-linear BT.2020 signal.
	/// @return Linear BT.2020 tristimulus.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_inverse_oetf_precise(const math::vec3<T>& x)
	{
		return bt2020_inverse_oetf(x, T{1} + T{5.5} * T{0.018053968510807}, T{0.018053968510807});
	}

	/// ITU-R BT.2020 12-bit inverse OETF.
	/// @tparam T Scalar type.
	/// @param x Non-linear BT.2020 signal.
	/// @return Linear BT.2020 tristimulus.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_inverse_oetf_12_bits(const math::vec3<T>& x)
	{
		return bt2020_inverse_oetf(x, T{1.0993}, T{0.0181});
	}

	/// ITU-R BT.2020 10-bit inverse OETF.
	/// @tparam T Scalar type.
	/// @param x Non-linear BT.2020 signal.
	/// @return Linear BT.2020 tristimulus.
	template <class T>
	[[nodiscard]] math::vec3<T> bt2020_inverse_oetf_10_bits(const math::vec3<T>& x)
	{
		return bt2020_inverse_oetf(x, T{1.099}, T{0.018});
	}

	/// ITU-R BT.2020 color space.
	/// @tparam T Scalar type.
	template <class T>
	inline constexpr rgb_color_space<T> bt2020
	{
		{T{0.7080}, T{0.2920}},
		{T{0.1700}, T{0.7970}},
		{T{0.1310}, T{0.0460}},
		{T{0.3127}, T{0.3290}},
		&bt2020_oetf_10_bits<T>,
		&bt2020_inverse_oetf_10_bits<T>
	};

	/// @}
}

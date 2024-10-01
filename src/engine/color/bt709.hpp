// SPDX-FileCopyrightText: 2024 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_COLOR_BT709_HPP
#define ANTKEEPER_COLOR_BT709_HPP

#include <engine/color/rgb.hpp>
#include <cmath>

namespace color {

/// @name ITU-R BT.709 color space
/// @{

/// ITU-R BT.709 Opto-Electronic Transfer Function (OETF).
/// @tparam T Scalar type.
/// @param x Linear BT.709 tristimulus.
/// @return Non-linear BT.709 signal.
template <class T>
[[nodiscard]] math::vec3<T> bt709_oetf(const math::vec3<T>& x)
{
	auto f = [](T x) -> T
	{
		return x < T{0.018} ? T{4.5} * x : T{1.099} * std::pow(x, T{0.45}) - T{0.099};
	};
	
	return {f(x[0]), f(x[1]), f(x[2])};
}

/// ITU-R BT.709 inverse OETF.
/// @tparam T Scalar type.
/// @param x Non-linear BT.709 signal.
/// @return Linear BT.709 tristimulus.
template <class T>
[[nodiscard]] math::vec3<T> bt709_inverse_oetf(const math::vec3<T>& x)
{
	const auto oetf_beta = T{1.099} * std::pow(T{0.018}, T{0.45}) - T{0.099};

	auto f = [oetf_beta](T x) -> T
	{
		return x < oetf_beta ? x / T{4.5} : std::pow((x + T{0.099}) / T{1.099}, T{1} / T{0.45});
	};
	
	return {f(x[0]), f(x[1]), f(x[2])};
}

/// ITU-R BT.709 color space.
/// @tparam T Scalar type.
template <class T>
constexpr rgb_color_space<T> bt709
{
	{T{0.6400}, T{0.3300}},
	{T{0.3000}, T{0.6000}},
	{T{0.1500}, T{0.0600}},
	{T{0.3127}, T{0.3290}},
	&bt709_oetf<T>,
	&bt709_inverse_oetf<T>
};

/// @}

} // namespace color

#endif // ANTKEEPER_COLOR_BT709_HPP

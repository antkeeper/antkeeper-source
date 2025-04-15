// SPDX-FileCopyrightText: 2025 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

export module engine.color.aces;
export import engine.color.rgb;
export import engine.math.matrix;
export import engine.math.vector;

export namespace engine::color
{
	/// @name ACES color spaces
	/// @{

	/// CIE xy chromaticity coordinates of the ACES white point (~D60).
	/// @tparam T Scalar type.
	template <class T>
	inline constexpr math::vec2<T> aces_white_point = {T{0.32168}, T{0.33767}};

	/// ACES AP0 color space.
	/// @tparam T Scalar type.
	template <class T>
	inline constexpr rgb_color_space<T> aces_ap0
	{
		{T{0.7347}, T{ 0.2653}},
		{T{0.0000}, T{ 1.0000}},
		{T{0.0001}, T{-0.0770}},
		aces_white_point<T>,
		nullptr,
		nullptr
	};

	/// ACES AP1 color space.
	/// @tparam T Scalar type.
	template <class T>
	inline constexpr rgb_color_space<T> aces_ap1
	{
		{T{0.713}, T{0.293}},
		{T{0.165}, T{0.830}},
		{T{0.128}, T{0.044}},
		aces_white_point<T>,
		nullptr,
		nullptr
	};

	/// Constructs a saturation adjustment matrix.
	/// @tparam T Element type.
	/// @param s Saturation adjustment factor.
	/// @param to_y Color space to CIE XYZ luminance vector.
	/// @return Saturation adjustment matrix.
	template <class T>
	[[nodiscard]] constexpr math::mat3<T> aces_adjust_saturation(T s, const math::vec3<T>& to_y) noexcept
	{
		const math::vec3<T> v = to_y * (T{1} - s);
		return math::mat3<T>
		{
			v[0] + s, v[0],     v[0],
			v[1],     v[1] + s, v[1],
			v[2],     v[2],     v[2] + s
		};
	}

	/// ACES AP1 RRT saturation adjustment matrix.
	/// @tparam T Element type.
	template <class T>
	inline constexpr math::mat3<T> aces_ap1_rrt_sat = aces_adjust_saturation(T{0.96}, aces_ap1<T>.to_y);

	/// ACES AP1 ODT saturation adjustment matrix.
	/// @tparam T Element type.
	template <class T>
	inline constexpr math::mat3<T> aces_ap1_odt_sat = aces_adjust_saturation(T{0.93}, aces_ap1<T>.to_y);

	/// @}
}

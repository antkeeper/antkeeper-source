// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_COLOR_CAT_HPP
#define ANTKEEPER_COLOR_CAT_HPP

#include <engine/math/matrix.hpp>
#include <engine/math/vector.hpp>

namespace color {

/// @name Chromatic adaption transforms (CAT)
/// @{

/**
 * Bradford cone response matrix.
 *
 * @see Specification ICC.1:2010 (Profile version 4.3.0.0). Image technology colour management — Architecture, profile format, and data structure, Annex E.3, pp. 102.
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 */
template <class T>
constexpr math::mat3<T> bradford_cone_response =
{
	T{ 0.8951}, T{-0.7502}, T{ 0.0389},
	T{ 0.2664}, T{ 1.7135}, T{-0.0685},
	T{-0.1614}, T{ 0.0367}, T{ 1.0296}
};

/**
 * von Kries cone response matrix.
 *
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 */
template <class T>
constexpr math::mat3<T> von_kries_cone_response =
{
	T{ 0.40024}, T{-0.22630}, T{0.00000},
	T{ 0.70760}, T{ 1.16532}, T{0.00000},
	T{-0.08081}, T{ 0.04570}, T{0.91822}
};

/**
 * XYZ scaling cone response matrix.
 *
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 */
template <class T>
constexpr math::mat3<T> xyz_scaling_cone_response = math::identity<math::mat3<T>>;

/**
 * Constructs a chromatic adaptation transform (CAT) matrix.
 *
 * @param w0 CIE xy chromaticity coordinates of the source illuminant.
 * @param w1 CIE xy chromaticity coordinates of the destination illuminant.
 * @param cone_response Cone response matrix.
 *
 * @return CAT matrix.
 *
 * @see Specification ICC.1:2010 (Profile version 4.3.0.0). Image technology colour management — Architecture, profile format, and data structure, Annex E.3, pp. 102.
 * @see http://www.brucelindbloom.com/index.html?Eqn_ChromAdapt.html
 */
template <class T>
[[nodiscard]] constexpr math::mat3<T> cat_matrix(const math::vec2<T>& w0, const math::vec2<T>& w1, const math::mat3<T>& cone_response = bradford_cone_response<T>) noexcept
{
	// Convert CIE xy chromaticity coordinates to CIE XYZ colors
	const math::vec3<T> w0_xyz = {w0[0] / w0[1], T{1}, (T{1} - w0[0] - w0[1]) / w0[1]};
	const math::vec3<T> w1_xyz = {w1[0] / w1[1], T{1}, (T{1} - w1[0] - w1[1]) / w1[1]};
	
	// Calculate cone response of CIE XYZ colors
	const math::vec3<T> w0_cone_response = cone_response * w0_xyz;
	const math::vec3<T> w1_cone_response = cone_response * w1_xyz;
	
	const math::mat3<T> scale =
	{
		w1_cone_response[0] / w0_cone_response[0], T{0}, T{0},
		T{0}, w1_cone_response[1] / w0_cone_response[1], T{0},
		T{0}, T{0}, w1_cone_response[2] / w0_cone_response[2],
	};
	
	return math::inverse(cone_response) * scale * cone_response;
}

/// @}

} // namespace color

#endif // ANTKEEPER_COLOR_CAT_HPP

// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_NUMBERS_HPP
#define ANTKEEPER_MATH_NUMBERS_HPP

#include <limits>
#include <numbers>

namespace math {

/// Mathematical constants.
namespace numbers {

/// Positive infinity.
template <class T>
inline constexpr T inf = std::numeric_limits<T>::infinity();

/// e.
template <class T>
inline constexpr T e = std::numbers::e_v<T>;

/// log2(e).
template <class T>
inline constexpr T log2_e = std::numbers::log2e_v<T>;

/// log10(e).
template <class T>
inline constexpr T log10_e = std::numbers::log10e_v<T>;

/// Pi.
template <class T>
inline constexpr T pi = std::numbers::pi_v<T>;

/// Pi * 2.
template <class T>
inline constexpr T two_pi = pi<T> * T{2};

/// Pi * 4.
template <class T>
inline constexpr T four_pi = pi<T> * T{4};

/// Pi / 2.
template <class T>
inline constexpr T half_pi = pi<T> / T{2};

/// 1 / Pi.
template <class T>
inline constexpr T inv_pi = std::numbers::inv_pi_v<T>;

/// 1 / sqrt(Pi).
template <class T>
inline constexpr T inv_sqrt_pi = std::numbers::inv_sqrtpi_v<T>;

/// Pi^2.
template <class T>
inline constexpr T sqr_pi = std::numbers::pi_v<T> * std::numbers::pi_v<T>;

/// ln(2).
template <class T>
inline constexpr T ln_2 = std::numbers::ln2_v<T>;

/// ln(10).
template <class T>
inline constexpr T ln_10 = std::numbers::ln10_v<T>;

/// sqrt(0.5)
template <class T>
inline constexpr T sqrt_half = T{0.70710678118654752440084436210485};

/// sqrt(2)
template <class T>
inline constexpr T sqrt_2 = std::numbers::sqrt2_v<T>;

/// sqrt(3)
template <class T>
inline constexpr T sqrt_3 = std::numbers::sqrt3_v<T>;

/// 1 / sqrt(3)
template <class T>
inline constexpr T inv_sqrt_3 = std::numbers::inv_sqrt3_v<T>;

/// sqrt(5)
template <class T>
inline constexpr T sqrt_5 = T{2.2360679774997896964091736687313};

/// Euler–Mascheroni constant.
template <class T>
inline constexpr T egamma = std::numbers::egamma_v<T>;

/// Golden ratio constant.
template <class T>
inline constexpr T phi = std::numbers::phi_v<T>;

/// Degrees-to-radians conversion factor.
template <class T>
inline constexpr T deg2rad = pi<T> / T{180};

/// Radians-to-degrees conversion factor.
template <class T>
inline constexpr T rad2deg = T{180} / pi<T>;

} // namespace numbers

// Bring math::numbers into math namespace
using namespace numbers;

} // namespace math

#endif // ANTKEEPER_MATH_NUMBERS_HPP

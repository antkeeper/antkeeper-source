// SPDX-FileCopyrightText: 2023 C. J. Howard
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ANTKEEPER_MATH_RANDOM_HPP
#define ANTKEEPER_MATH_RANDOM_HPP

#include <cstddef>
#include <type_traits>

namespace math {

/**
 * Generates a pseudo-random floating point number on `[start, end)` using std::rand().
 *
 * @warning Don't forget to seed with std::srand() before using!
 *
 * @param start Start of the range (inclusive).
 * @param end End of the range (exclusive).
 * @return Pseudo-random floating point number.
 */
template <typename T = float>
[[nodiscard]] T random(T start, T end)
{
	static_assert(std::is_floating_point<T>::value);
	constexpr T rand_max_inverse = T(1) / static_cast<T>(RAND_MAX);
    return static_cast<T>(std::rand()) * rand_max_inverse * (end - start) + start;
}

} // namespace math

#endif // ANTKEEPER_MATH_RANDOM_HPP

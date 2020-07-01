/*
 * Copyright (C) 2020  Christopher J. Howard
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

#ifndef ANTKEEPER_TWEEN_HPP
#define ANTKEEPER_TWEEN_HPP

#include <algorithm>
#include <functional>
#include <type_traits>

/**
 * Linearly interpolates between two values.
 *
 * @param x Start of the range in which to interpolate.
 * @param y End of the range in which to interpolate.
 * @param a Value used to interpolate between @p x and @p y.
 * @return Interpolated value.
 */
template <class T>
T tween_default_lerp(const T& x, const T& y, float a);

/**
 * Container which stores two states along with an interpolator, for quick and easy tweening.
 *
 * @tparam T Value type.
 * @tparam Interpolator Interpolator function or function object type.
 */
template <class T, class Interpolator = typename std::function<std::remove_pointer<T>::type(const T&, const T&, float)>>
class tween
{
public:
	typedef typename std::remove_pointer<T>::type value_type;
	typedef typename std::decay<Interpolator>::type interpolator_type;

	/**
	 * Creates a tween.
	 *
	 * @param state0 Initial value of state 0.
	 * @param state1 Initial value of state 1.
	 * @param interpolator Function or function object that will be used to interpolate between states 0 and 1.
	 */
	explicit tween(const T& state0, const T& state1, const interpolator_type& interpolator = tween_default_lerp<T>);

	/**
	 * Creates a tween.
	 *
	 * @param value Initial value of states 0 and 1.
	 * @param interpolator Function or function object that will be used to interpolate between states 0 and 1.
	 */
	explicit tween(const T& value, const interpolator_type& interpolator = tween_default_lerp<T>);

	/**
	 * Creates a tween.
	 *
	 * @param interpolator Function or function object that will be used to interpolate between states 0 and 1.
	 */
	explicit tween(const interpolator_type& interpolator = tween_default_lerp<T>);

	/**
	 * Returns a reference to the specified tween state.
	 *
	 * @param state Index of a tween state. Should be either `0` or `1`.
	 * @return Reference to the specified tween state.
	 */
	const T& operator[](int state) const;

	/// @copydoc tween::operator[](int) const
	T& operator[](int state);

	/**
	 * Returns an interpolated state between state 0 and state 1.
	 *
	 * @param a Interpolation factor. Should be on `[0.0, 1.0]`.
	 * @return Interpolated state.
	 */
	value_type interpolate(float a) const;

	/**
	 * Returns the function or function object that is used to interpolate between states 0 and 1.
	 */
	const interpolator_type& get_interpolator() const;

	/**
	 * Sets state 0 = state 1.
	 */
	void update();

	/**
	 * Swaps state 0 and state 1.
	 */
	void swap();

private:
	interpolator_type interpolator;
	T state0;
	T state1;
};

template <class T>
inline T tween_default_lerp(const T& x, const T& y, float a)
{
	return x * (1.0f - a) + y * a;
}

template <class T, class Interpolator>
tween<T, Interpolator>::tween(const T& value, const interpolator_type& interpolator):
	interpolator(interpolator),
	state0(value),
	state1(value)
{}

template <class T, class Interpolator>
tween<T, Interpolator>::tween(const T& state0, const T& state1, const interpolator_type& interpolator):
	interpolator(interpolator),
	state0(state0),
	state1(state1)
{}

template <class T, class Interpolator>
tween<T, Interpolator>::tween(const interpolator_type& interpolator):
	interpolator(interpolator)
{}

template <class T, class Interpolator>
inline const T& tween<T, Interpolator>::operator[](int state) const
{
	return (state <= 0) ? state0 : state1;
}

template <class T, class Interpolator>
inline T& tween<T, Interpolator>::operator[](int state)
{
	return (state <= 0) ? state0 : state1;
}

template <class T, class Interpolator>
inline typename tween<T, Interpolator>::value_type tween<T, Interpolator>::interpolate(float a) const
{
	return interpolator(state0, state1, a);
}

template <class T, class Interpolator>
inline const typename tween<T, Interpolator>::interpolator_type& tween<T, Interpolator>::get_interpolator() const
{
	return interpolator;
}

template <class T, class Interpolator>
inline void tween<T, Interpolator>::update()
{
	state0 = state1;
}

template <class T, class Interpolator>
inline void tween<T, Interpolator>::swap()
{
	std::swap(state0, state1);
}

#endif // ANTKEEPER_TWEEN_HPP


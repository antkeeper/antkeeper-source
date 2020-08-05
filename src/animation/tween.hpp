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
T tween_default_interpolator(const T& x, const T& y, double a);

/**
 * Container which stores two states along with an interpolator, for quick and easy tweening.
 *
 * @tparam T Value type.
 */
template <class T>
class tween
{
public:
	//typedef typename std::remove_pointer<T>::type value_type;
	typedef T value_type;
	typedef typename std::decay<std::function<value_type(const T&, const T&, double)>>::type interpolator_type;
	
	/**
	 * Creates a tween.
	 *
	 * @param state0 Initial value of state 0.
	 * @param state1 Initial value of state 1.
	 * @param interpolator Function or function object that will be used to interpolate between states 0 and 1.
	 */
	explicit tween(const T& state0, const T& state1, const interpolator_type& interpolator = tween_default_interpolator<T>);

	/**
	 * Creates a tween.
	 *
	 * @param value Initial value of states 0 and 1.
	 * @param interpolator Function or function object that will be used to interpolate between states 0 and 1.
	 */
	explicit tween(const T& value, const interpolator_type& interpolator = tween_default_interpolator<T>);

	/**
	 * Creates a tween.
	 *
	 * @param interpolator Function or function object that will be used to interpolate between states 0 and 1.
	 */
	explicit tween(const interpolator_type& interpolator = tween_default_interpolator<T>);

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
	value_type interpolate(double a) const;
	
	/**
	 * Sets the function object used to interpolate between states 0 and 1.
	 *
	 * @param interpolator Interpolator function object.
	 */
	void set_interpolator(const interpolator_type& interpolator);

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
inline T tween_default_interpolator(const T& x, const T& y, double a)
{
	//return x * (1.0 - a) + y * a;
	return y;
}

template <>
inline float tween_default_interpolator<float>(const float& x, const float& y, double a)
{
	return (y - x) * a + x;
}

template <class T>
tween<T>::tween(const T& value, const interpolator_type& interpolator):
	interpolator(interpolator),
	state0(value),
	state1(value)
{}

template <class T>
tween<T>::tween(const T& state0, const T& state1, const interpolator_type& interpolator):
	interpolator(interpolator),
	state0(state0),
	state1(state1)
{}

template <class T>
tween<T>::tween(const interpolator_type& interpolator):
	interpolator(interpolator)
{}

template <class T>
inline const T& tween<T>::operator[](int state) const
{
	return (state <= 0) ? state0 : state1;
}

template <class T>
inline T& tween<T>::operator[](int state)
{
	return (state <= 0) ? state0 : state1;
}

template <class T>
inline typename tween<T>::value_type tween<T>::interpolate(double a) const
{
	return interpolator(state0, state1, a);
}

template <class T>
inline void tween<T>::set_interpolator(const interpolator_type& interpolator)
{
	this->interpolator = interpolator;
}

template <class T>
inline const typename tween<T>::interpolator_type& tween<T>::get_interpolator() const
{
	return interpolator;
}

template <class T>
inline void tween<T>::update()
{
	state0 = state1;
}

template <class T>
inline void tween<T>::swap()
{
	std::swap(state0, state1);
}

#endif // ANTKEEPER_TWEEN_HPP

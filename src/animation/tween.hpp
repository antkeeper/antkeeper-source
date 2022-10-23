/*
 * Copyright (C) 2021  Christopher J. Howard
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
#include <stdexcept>
#include <type_traits>

/**
 * Container which stores two states along with an interpolator, for quick and easy tween<T, S>ing.
 *
 * @tparam T Value type.
 * @tparam S Scalar type.
 */
template <class T, class S = float>
class tween
{
public:
	static_assert(std::is_scalar<S>::value);
	
	typedef T value_type;
	typedef S scalar_type;
	typedef typename std::decay<std::function<value_type(const value_type&, const value_type&, scalar_type)>>::type interpolator_type;
	
private:
	/// Throws an error if no interpolator is set.
	static value_type interpolator_error(const value_type&, const value_type&, scalar_type);
	
public:
	
	/**
	 * Creates a tween.
	 *
	 * @param state0 Initial value of state 0.
	 * @param state1 Initial value of state 1.
	 * @param interpolator Function used to interpolate between states 0 and 1.
	 */
	tween(const value_type& state0, const value_type& state1, interpolator_type interpolator = tween<T, S>::interpolator_error);
	
	/**
	 * Creates a tween.
	 *
	 * @param value Initial value of states 0 and 1.
	 * @param interpolator Function used to interpolate between states 0 and 1.
	 */
	explicit tween(const value_type& value, interpolator_type interpolator = tween<T, S>::interpolator_error);
	
	/**
	 * Creates a tween.
	 */
	tween();

	/**
	 * Returns a reference to the specified tween state.
	 *
	 * @param i Index of a tween state. Should be either `0` or `1`.
	 * @return Reference to the specified tween state.
	 */
	const value_type& operator[](int i) const;

	/// @copydoc tween<T, S>::operator[](int) const
	value_type& operator[](int i);
	
	/// @copydoc tween<T, S>::interpolate(scalar_type) const
	value_type operator[](scalar_type a) const;

	/**
	 * Returns an interpolated state between state 0 and state 1. If no interpolator is set, state 1 will be returned.
	 *
	 * @param a Interpolation factor on `[0.0, 1.0]`.
	 * @return Interpolated state, or state 1 if no interpolator is set.
	 */
	value_type interpolate(scalar_type a) const;
	
	/**
	 * Sets the function used to interpolate between states 0 and 1.
	 *
	 * @param interpolator Interpolator function.
	 */
	void set_interpolator(const interpolator_type& interpolator);

	/**
	 * Returns the function used to interpolate between states 0 and 1.
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
	value_type states[2];
	interpolator_type interpolator;
};

template <class T, class S>
typename tween<T, S>::value_type tween<T, S>::interpolator_error(const value_type&, const value_type&, scalar_type)
{
	throw std::runtime_error("tween interpolator not set");
}

template <class T, class S>
tween<T, S>::tween(const value_type& value, interpolator_type interpolator):
	states{value, value},
	interpolator(interpolator)
{}

template <class T, class S>
tween<T, S>::tween(const value_type& state0, const value_type& state1, interpolator_type interpolator):
	states{state0, state1},
	interpolator(interpolator)
{}

template <class T, class S>
tween<T, S>::tween():
	interpolator(nullptr)
{}

template <class T, class S>
inline const typename tween<T, S>::value_type& tween<T, S>::operator[](int i) const
{
	return states[i];
}

template <class T, class S>
inline typename tween<T, S>::value_type& tween<T, S>::operator[](int i)
{
	return states[i];
}

template <class T, class S>
inline typename tween<T, S>::value_type tween<T, S>::operator[](scalar_type a) const
{
	return interpolate(a);
}

template <class T, class S>
inline typename tween<T, S>::value_type tween<T, S>::interpolate(scalar_type a) const
{
	return interpolator(states[0], states[1], a);
}

template <class T, class S>
inline void tween<T, S>::set_interpolator(const interpolator_type& interpolator)
{
	this->interpolator = interpolator;
}

template <class T, class S>
inline const typename tween<T, S>::interpolator_type& tween<T, S>::get_interpolator() const
{
	return interpolator;
}

template <class T, class S>
inline void tween<T, S>::update()
{
	states[0] = states[1];
}

template <class T, class S>
inline void tween<T, S>::swap()
{
	std::swap(states[0], states[1]);
}

#endif // ANTKEEPER_TWEEN_HPP
